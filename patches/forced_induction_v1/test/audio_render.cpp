// Offline audio render (diagnostic/comparison tool, no assertions).
//
// Runs an engine script through the stock simulator and the real synthesizer
// exactly as the GUI sets them up (EngineSimApplication::loadEngine: script
// simulation frequency, jitter/noise/HF audio parameters, per-exhaust impulse
// responses, synthesizer rendering thread), but without a real-time deadline,
// so the output has no buffer underruns. Writes 16-bit mono 44.1 kHz WAV.
//
// Schedule (simulated time): starter 0..crank s, speed control 0 until
// rev_start, 1.0 until rev_end, then 0 until end.
//
// usage: engine-sim-audio-render <script.mr> <out.wav>
//            [--crank 3] [--rev-start 6] [--rev-end 14] [--end 20] [--rev-level 1.0]
//            [--knock-level L] [--turbo-level L]   (override global layer levels)
//            [--dyno T RPM]   (from time T hold RPM; engaged while turning forward)

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "../scripting/include/compiler.h"
#include "../include/engine.h"
#include "../include/simulator.h"
#include "../include/transmission.h"
#include "../include/vehicle.h"
#include "../include/units.h"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

#include <delta-studio/include/yds_windows_audio_wave_file.h>

namespace {
void writeWav(const std::string &path, const std::vector<int16_t> &s, int rate) {
    FILE *f = std::fopen(path.c_str(), "wb");
    if (!f) return;
    const uint32_t dataBytes = static_cast<uint32_t>(s.size() * 2);
    auto u32 = [f](uint32_t v) { std::fwrite(&v, 4, 1, f); };
    auto u16 = [f](uint16_t v) { std::fwrite(&v, 2, 1, f); };
    std::fwrite("RIFF", 1, 4, f); u32(36 + dataBytes); std::fwrite("WAVE", 1, 4, f);
    std::fwrite("fmt ", 1, 4, f); u32(16); u16(1); u16(1); u32(rate); u32(rate * 2); u16(2); u16(16);
    std::fwrite("data", 1, 4, f); u32(dataBytes);
    std::fwrite(s.data(), 2, s.size(), f);
    std::fclose(f);
}
}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::fprintf(stderr, "usage: engine-sim-audio-render <script.mr> <out.wav> [--crank S] [--rev-start S] [--rev-end S] [--end S]\n");
        return 2;
    }
    double crank = 3.0, revStart = 6.0, revEnd = 14.0, end = 20.0, revLevel = 1.0;
    double knockLevel = -1.0, turboLevel = -1.0;
    double dynoTime = -1.0, dynoRpm = 0.0;
    for (int i = 3; i + 1 < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--crank") crank = std::atof(argv[++i]);
        else if (a == "--rev-start") revStart = std::atof(argv[++i]);
        else if (a == "--rev-level") revLevel = std::atof(argv[++i]);
        else if (a == "--rev-end") revEnd = std::atof(argv[++i]);
        else if (a == "--end") end = std::atof(argv[++i]);
        else if (a == "--knock-level") knockLevel = std::atof(argv[++i]);
        else if (a == "--turbo-level") turboLevel = std::atof(argv[++i]);
        else if (a == "--dyno" && i + 2 < argc) { dynoTime = std::atof(argv[++i]); dynoRpm = std::atof(argv[++i]); }
    }

    es_script::Compiler compiler;
    compiler.initialize();
    if (!compiler.compile(argv[1])) { std::fprintf(stderr, "compile failed\n"); return 10; }
    auto out = compiler.execute();
    Engine *engine = out.engine;
    if (!engine) { std::fprintf(stderr, "no engine\n"); return 11; }

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

    // As EngineSimApplication::loadEngine().
    Simulator *sim = engine->createSimulator(vehicle, transmission);
    engine->calculateDisplacement();
    sim->setSimulationFrequency(engine->getSimulationFrequency());
    Synthesizer::AudioParameters audioParams = sim->synthesizer().getAudioParameters();
    audioParams.inputSampleNoise = static_cast<float>(engine->getInitialJitter());
    audioParams.airNoise = static_cast<float>(engine->getInitialNoise());
    audioParams.dF_F_mix = static_cast<float>(engine->getInitialHighFrequencyGain());
    if (knockLevel >= 0.0) audioParams.combustionNoiseLevel = static_cast<float>(knockLevel);
    if (turboLevel >= 0.0) audioParams.turboSoundLevel = static_cast<float>(turboLevel);
    sim->synthesizer().setAudioParameters(audioParams);
    for (int i = 0; i < engine->getExhaustSystemCount(); ++i) {
        ImpulseResponse *response = engine->getExhaustSystem(i)->getImpulseResponse();
        ysWindowsAudioWaveFile waveFile;
        waveFile.OpenFile(response->getFilename().c_str());
        waveFile.InitializeInternalBuffer(waveFile.GetSampleCount());
        waveFile.FillBuffer(0);
        waveFile.CloseFile();
        sim->synthesizer().initializeImpulseResponse(
            reinterpret_cast<const int16_t *>(waveFile.GetBuffer()),
            waveFile.GetSampleCount(), response->getVolume(), i);
        waveFile.DestroyInternalBuffer();
    }
    sim->startAudioRenderingThread();

    engine->getIgnitionModule()->m_enabled = true;
    const double dt = sim->getTimestep();
    double t = 0.0;
    std::vector<int16_t> samples;
    samples.reserve(static_cast<size_t>(44100 * (end + 2.0)));
    int16_t chunk[4096];
    auto drain = [&]() {
        int n;
        while ((n = sim->readAudioOutput(4096, chunk)) > 0) samples.insert(samples.end(), chunk, chunk + n);
    };

    while (t < end) {
        sim->m_starterMotor.m_enabled = t < crank;
        engine->setSpeedControl((t >= revStart && t < revEnd) ? revLevel : 0.0);
        if (dynoTime >= 0.0 && t >= dynoTime && !sim->m_dyno.m_enabled) {
            // Dynamometer::calculate holds |m_rotationSpeed| in the current
            // rotation direction; engage it while the crank turns forward.
            sim->m_dyno.m_rotationSpeed = units::rpm(dynoRpm);
            sim->m_dyno.m_maxTorque = units::torque(50000.0, units::ft_lb);
            sim->m_dyno.m_hold = true;
            sim->m_dyno.m_enabled = true;
        }
        sim->startFrame(1.0 / 60.0);
        while (sim->simulateStep()) t += dt;
        sim->endFrame();
        sim->synthesizer().waitProcessed();
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        drain();
    }
    for (int i = 0; i < 50; ++i) { std::this_thread::sleep_for(std::chrono::milliseconds(5)); drain(); }
    sim->endAudioRenderingThread();

    // The synthesizer starts with one buffer (44100 samples) of silence.
    const size_t skip = std::min<size_t>(44100, samples.size());
    std::vector<int16_t> trimmed(samples.begin() + skip, samples.end());
    writeWav(argv[2], trimmed, 44100);
    std::printf("engine=%s frequency_hz=%.0f simulated_s=%.2f audio_s=%.2f file=%s\n",
        engine->getName().c_str(), engine->getSimulationFrequency(), t, trimmed.size() / 44100.0, argv[2]);
    compiler.destroy();
    return 0;
}
