// Real-time budget benchmark (diagnostic tool, no assertions).
//
// Measures, for one engine script, whether the GUI pipeline can run in real
// time on this machine:
//   - physics: wall time per simulation step with the stock simulator,
//     including writeToSynthesizer() (the audio-input path the GUI runs);
//   - telemetry: the extra per-step cost of TelemetryLog::sampleStep();
//   - audio thread: wall time to render one second of 44.1 kHz output through
//     the same per-channel filter chain as Synthesizer::renderAudio(), using
//     each exhaust system's own impulse response (ConvolutionFilter).
// Budget: physics needs frequency x (time per step) < 1 s per second; the
// audio thread needs its render time < 1 s per second of audio.
//
// usage: engine-sim-realtime-bench <script.mr> [--frequency N]

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "../scripting/include/compiler.h"
#include "../include/engine.h"
#include "../include/simulator.h"
#include "../include/gas_system.h"
#include "../include/piston_engine_simulator.h"
#include "../include/transmission.h"
#include "../include/vehicle.h"
#include "../include/convolution_filter.h"
#include "../include/telemetry_log.h"
#include "../include/units.h"

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include <delta-studio/include/yds_windows_audio_wave_file.h>

namespace {
using Clock = std::chrono::steady_clock;

double secondsSince(Clock::time_point t0) {
    return std::chrono::duration<double>(Clock::now() - t0).count();
}

// Section timing: verbatim copy of PistonEngineSimulator::simulateStep_()
// with timers between the production calls (as in the cylinder probe).
struct Sections {
    double ignition = 0, chamberUpdate = 0, exhaust = 0, intake = 0, chamberFlow = 0,
        forcedInduction = 0, synth = 0, total = 0;
};

class TimedSimulator : public PistonEngineSimulator {
public:
    Sections s;
protected:
    void simulateStep_() override {
        auto t = Clock::now();
        auto lap = [&t](double &acc) { const auto n = Clock::now(); acc += std::chrono::duration<double>(n - t).count(); t = n; };
        const double timestep = getTimestep();
        IgnitionModule *im = m_engine->getIgnitionModule();
        im->update(timestep);
        const int cylinderCount = m_engine->getCylinderCount();
        lap(s.ignition);
        for (int i = 0; i < cylinderCount; ++i) {
            m_engine->getChamber(i)->resetCombustionPressureRiseRate();
            if (im->getIgnitionEvent(i)) {
                if (m_engine->isCompressionIgnition()) {
                    m_engine->getChamber(i)->beginCompressionIgnitionEvent(m_engine->getFuelMassPerCycleCommand());
                }
                else {
                    m_engine->getChamber(i)->ignite();
                }
            }
            m_engine->getChamber(i)->update(timestep);
        }
        for (int i = 0; i < cylinderCount; ++i) {
            m_engine->getChamber(i)->resetLastTimestepExhaustFlow();
            m_engine->getChamber(i)->resetLastTimestepIntakeFlow();
        }
        lap(s.chamberUpdate);
        const int exhaustSystemCount = m_engine->getExhaustSystemCount();
        const int intakeCount = m_engine->getIntakeCount();
        const double fluidTimestep = timestep / m_fluidSimulationSteps;
        for (int i = 0; i < m_fluidSimulationSteps; ++i) {
            for (int j = 0; j < exhaustSystemCount; ++j) m_engine->getExhaustSystem(j)->process(fluidTimestep);
            lap(s.exhaust);
            for (int j = 0; j < intakeCount; ++j) m_engine->getIntake(j)->process(fluidTimestep);
            lap(s.intake);
            for (int j = 0; j < cylinderCount; ++j) m_engine->getChamber(j)->flow(fluidTimestep);
            lap(s.chamberFlow);
            m_engine->processForcedInduction(fluidTimestep);
            for (int j = 0; j < intakeCount; ++j) m_engine->getIntake(j)->m_flowRate += m_engine->getIntake(j)->m_flow;
            lap(s.forcedInduction);
        }
        im->resetIgnitionEvents();
    }
    void writeToSynthesizer() override {
        const auto t0 = Clock::now();
        PistonEngineSimulator::writeToSynthesizer();
        s.synth += std::chrono::duration<double>(Clock::now() - t0).count();
    }
};

// Steps the simulator for `steps` steps (starter as given); returns wall seconds.
double runSteps(Simulator *sim, long long steps, TelemetryLog *log) {
    long long done = 0;
    const auto t0 = Clock::now();
    while (done < steps) {
        sim->startFrame(1.0 / 60.0);
        while (done < steps && sim->simulateStep()) {
            if (log) log->sampleStep();
            ++done;
        }
        sim->endFrame();
        if (log) log->endFrame();
        // Drain the synthesizer input as the audio thread would, so the
        // latency controller does not throttle the step count.
        int16_t sink[4096];
        while (sim->readAudioOutput(4096, sink) > 0) {}
    }
    return secondsSince(t0);
}
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: engine-sim-realtime-bench <script.mr> [--frequency N]\n");
        return 2;
    }
    int frequency = 0;
    for (int i = 2; i < argc; ++i) {
        if (std::string(argv[i]) == "--frequency" && i + 1 < argc) frequency = std::atoi(argv[i + 1]);
        if (std::string(argv[i]) == "--ideal-gas") gas_vibration::enabled = false;
    }

    es_script::Compiler compiler;
    compiler.initialize();
    if (!compiler.compile(argv[1])) { std::fprintf(stderr, "compile failed\n"); return 10; }
    auto out = compiler.execute();
    Engine *engine = out.engine;
    if (!engine) { std::fprintf(stderr, "no engine\n"); return 11; }

    // Same defaults as EngineSimApplication::loadScript() when a script
    // supplies no vehicle or transmission.
    Vehicle *vehicle = out.vehicle;
    if (!vehicle) {
        Vehicle::Parameters p;
        p.mass = units::mass(1597, units::kg);
        p.diffRatio = 3.42;
        p.tireRadius = units::distance(10, units::inch);
        p.dragCoefficient = 0.25;
        p.crossSectionArea = units::distance(6.0, units::foot) * units::distance(6.0, units::foot);
        p.rollingResistance = 2000.0;
        vehicle = new Vehicle;
        vehicle->initialize(p);
    }
    Transmission *transmission = out.transmission;
    if (!transmission) {
        static const double ratios[] = { 2.97, 2.07, 1.43, 1.00, 0.84, 0.56 };
        Transmission::Parameters p;
        p.GearCount = 6;
        p.GearRatios = ratios;
        p.MaxClutchTorque = units::torque(1000.0, units::ft_lb);
        transmission = new Transmission;
        transmission->initialize(p);
    }

    // Same construction sequence as Engine::createSimulator().
    TimedSimulator *timed = new TimedSimulator;
    Simulator::Parameters simulatorParams;
    simulatorParams.systemType = Simulator::SystemType::NsvOptimized;
    timed->initialize(simulatorParams);
    timed->loadSimulation(engine, vehicle, transmission);
    timed->setFluidSimulationSteps(8);
    Simulator *sim = timed;
    if (frequency <= 0) frequency = static_cast<int>(engine->getSimulationFrequency());
    sim->setSimulationFrequency(frequency);
    engine->setSpeedControl(0.0);
    engine->getIgnitionModule()->m_enabled = true;

    // Crank for 2 s, then run 2 s unaided so the measurement is of a running engine.
    sim->m_starterMotor.m_enabled = true;
    runSteps(sim, 2LL * frequency, nullptr);
    sim->m_starterMotor.m_enabled = false;
    runSteps(sim, 2LL * frequency, nullptr);

    const long long measureSteps = 2LL * frequency;
    timed->s = Sections{};
    const double physicsWall = runSteps(sim, measureSteps, nullptr);
    const Sections sec = timed->s;
    TelemetryLog log;
    log.open(engine, sim, "logs", 0.5);
    const double withLogWall = runSteps(sim, measureSteps, &log);
    log.close();

    const double usPerStep = 1e6 * physicsWall / measureSteps;
    const double logUsPerStep = 1e6 * (withLogWall - physicsWall) / measureSteps;
    const double physicsLoad = usPerStep * 1e-6 * frequency;

    // Audio thread: one second of output per exhaust channel through the
    // impulse-response convolution (the dominant per-sample cost).
    const int channels = engine->getExhaustSystemCount();
    double audioWall = 0.0;
    int totalTaps = 0;
    int sharedChannels = 0;
    std::vector<std::vector<float>> seen;
    for (int c = 0; c < channels; ++c) {
        ImpulseResponse *ir = engine->getExhaustSystem(c)->getImpulseResponse();
        ysWindowsAudioWaveFile wave;
        wave.OpenFile(ir->getFilename().c_str());
        wave.InitializeInternalBuffer(wave.GetSampleCount());
        wave.FillBuffer(0);
        wave.CloseFile();
        const int16_t *data = reinterpret_cast<const int16_t *>(wave.GetBuffer());
        unsigned int clipped = 0;
        for (unsigned int i = 0; i < wave.GetSampleCount(); ++i) {
            if (std::abs(data[i]) > 100) clipped = i + 1;
        }
        const int taps = static_cast<int>(std::min(10000U, clipped));
        std::vector<float> coefficients(taps);
        for (int i = 0; i < taps; ++i) {
            coefficients[i] = static_cast<float>(ir->getVolume()) * data[i] / 32767.0f;
        }
        wave.DestroyInternalBuffer();
        // The synthesizer shares one convolution among identical responses.
        bool shared = false;
        for (const auto &s : seen) shared = shared || s == coefficients;
        if (shared) { ++sharedChannels; continue; }
        seen.push_back(coefficients);
        totalTaps += taps;
        ConvolutionFilter filter;
        filter.initialize(taps);
        for (int i = 0; i < taps; ++i) filter.getImpulseResponse()[i] = coefficients[i];

        volatile float sink = 0.0f;
        const auto t0 = Clock::now();
        for (int n = 0; n < 44100; ++n) sink = sink + filter.f(static_cast<float>(std::sin(n * 0.01)));
        audioWall += secondsSince(t0);
        filter.destroy();
    }

    std::printf("engine=%s cylinders=%d exhaust_channels=%d frequency_hz=%d\n",
        engine->getName().c_str(), engine->getCylinderCount(), channels, frequency);
    std::printf("physics: %.1f us/step -> %.2f s of CPU per simulated second (load %.0f%% of one core)\n",
        usPerStep, physicsLoad, 100.0 * physicsLoad);
    {
        const double per = 1e6 / measureSteps;
        const double inStep = sec.ignition + sec.chamberUpdate + sec.exhaust + sec.intake
            + sec.chamberFlow + sec.forcedInduction + sec.synth;
        std::printf("physics breakdown (us/step): rigid-body+engine/vehicle %.1f | ignition %.1f | chamber update %.1f"
            " | exhaust process %.1f | intake process %.1f | chamber flow %.1f | forced induction %.1f | audio input %.1f\n",
            (physicsWall - inStep) * per, sec.ignition * per, sec.chamberUpdate * per, sec.exhaust * per,
            sec.intake * per, sec.chamberFlow * per, sec.forcedInduction * per, sec.synth * per);
    }
    std::printf("telemetry log: +%.2f us/step (+%.1f%%)\n", logUsPerStep, 100.0 * logUsPerStep / usPerStep);
    std::printf("audio thread: convolution %d taps total (%d channel(s) share a filter) -> %.2f s of CPU per second of audio (load %.0f%%)\n",
        totalTaps, sharedChannels, audioWall, 100.0 * audioWall);
    std::printf("realtime_ok physics=%s audio=%s\n", physicsLoad < 0.8 ? "yes" : "NO", audioWall < 0.8 ? "yes" : "NO");
    compiler.destroy();
    return 0;
}
