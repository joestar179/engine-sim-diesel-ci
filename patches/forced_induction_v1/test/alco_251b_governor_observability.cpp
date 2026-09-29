// Gate 6B: governor/control observability for the native 16-251B.
//
// Diagnostic only. It runs the corrected Gate 6 loaded-transient fixture
// (dyno held in the simulator's forward direction, release to idle), records
// governor state and reports a classification. It never asserts a physics
// outcome: the gate passes when the evidence is complete.

#include "../scripting/include/compiler.h"
#include "../include/engine.h"
#include "../include/simulator.h"
#include "../include/transmission.h"
#include "../include/vehicle.h"
#include "../include/units.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
constexpr int SimulationFrequency = 8000;

// Governor constants as declared by alco_16_251b_native.mr. The DieselGovernor
// is not reachable through a public accessor, so these are assumptions of this
// harness; the shadow model below verifies them against the observed rack.
constexpr double GovernorMinRpm = 400.0;
constexpr double GovernorMaxRpm = 1000.0;
constexpr double GovernorMinRate = -2.0;
constexpr double GovernorMaxRate = 2.0;
constexpr double GovernorKs = 0.008;
constexpr double GovernorKd = 120.0;
constexpr double GovernorGamma = 1.5;
constexpr double GovernorStartingRack = 0.20;

// Corrected Gate 6 fixture, shared with alco_251b_loaded_transient_validation.
constexpr double HeldRpm = 600.0;
constexpr double LowCommand =
    (HeldRpm - GovernorMinRpm) / (GovernorMaxRpm - GovernorMinRpm);
constexpr double HighCommand = 1.0;
constexpr double ReleaseCommand = 0.0;

// Evidence sampling.
constexpr double TraceInterval = 0.01;
constexpr double TransientCapture = 0.05;
constexpr double ShadowSettle = 0.05;

// Classification thresholds (reported with the evidence).
constexpr double HeldMeanTolerance = 0.01;
constexpr double HeldExcursionTolerance = 0.02;
constexpr double CommandTolerance = 1.0e-9;
constexpr double ShadowTolerance = 1.0e-6;
constexpr double NeutralErrorFraction = 0.01;
constexpr double ReleaseRetreat = 0.01;
constexpr double PersistentRateFraction = 0.01;

class EvidenceFailure : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

void require(bool condition, const std::string &message) {
    if (!condition) throw EvidenceFailure(message);
}

double clamp01(double x) { return std::max(0.0, std::min(1.0, x)); }

std::string errorLog() {
    std::ifstream log("error_log.log");
    std::ostringstream details;
    details << log.rdbuf();
    return details.str();
}

struct Runtime {
    es_script::Compiler compiler;
    Engine *engine = nullptr;
    Simulator *simulator = nullptr;

    void load(const char *script) {
        compiler.initialize();
        require(compiler.compile(script),
            "reference script did not compile: " + errorLog());
        auto output = compiler.execute();
        engine = output.engine;
        require(engine != nullptr, "reference script produced no engine: " + errorLog());
        require(output.vehicle != nullptr && output.transmission != nullptr,
            "reference load model is missing");
        simulator = engine->createSimulator(output.vehicle, output.transmission);
        require(simulator != nullptr, "reference script did not create a simulator");
        simulator->setSimulationFrequency(SimulationFrequency);
    }

    void finish() { compiler.destroy(); }
};

void requireNative251B(Runtime &runtime) {
    require(runtime.engine->getCylinderCount() == 16,
        "reference engine is not the 16-cylinder model");
    require(runtime.engine->isCompressionIgnition(),
        "16-251B is not using native compression ignition");
    require(runtime.engine->getIntakeCount() == 1,
        "16-251B must use its documented common intake");
    require(runtime.engine->getExhaustSystemCount() == 4,
        "16-251B must expose four pulse branches");
    ForcedInductionSystem *system = runtime.engine->getForcedInductionSystem();
    require(system->enabled() && system->groupCount() == 1,
        "16-251B must use one native TurboGroup");
    std::set<GasSystem *> scrolls;
    for (int i = 0; i < runtime.engine->getExhaustSystemCount(); ++i) {
        ExhaustSystem *exhaust = runtime.engine->getExhaustSystem(i);
        GasSystem *destination = runtime.engine->getExhaustDestination(exhaust);
        require(destination != exhaust->getSystem(),
            "16-251B pulse branch bypasses the turbine");
        scrolls.insert(destination);
    }
    require(scrolls.size() == 4, "16-251B pulse branches do not reach four scrolls");
}

// Replica of FuelRackGovernorModel::update driven by the observed crank speed.
struct ShadowGovernor {
    double rack = 0.0;
    double rate = 0.0;

    double update(double dt, double targetSpeed, double speed) {
        const double error = targetSpeed * targetSpeed - speed * speed;
        rate += dt * error * GovernorKs - dt * rate * GovernorKd;
        rate = std::max(GovernorMinRate, std::min(GovernorMaxRate, rate));
        rack = clamp01(rack + rate * dt);
        if (speed < 0.5 * units::rpm(GovernorMinRpm)) {
            rack = std::max(rack, std::pow(GovernorStartingRack, 1.0 / GovernorGamma));
        }
        return std::pow(rack, GovernorGamma);
    }
};

double notANumber() { return std::numeric_limits<double>::quiet_NaN(); }

struct PhaseStats {
    std::string name;
    bool measured = false;
    double commanded = 0.0;
    double seconds = 0.0;
    double heldTime = 0.0, signedTime = 0.0, errorTime = 0.0, rackTime = 0.0;
    double heldMin = std::numeric_limits<double>::infinity();
    double heldMax = -std::numeric_limits<double>::infinity();
    double errorMin = std::numeric_limits<double>::infinity();
    double errorMax = -std::numeric_limits<double>::infinity();
    double rackMin = std::numeric_limits<double>::infinity();
    double rackMax = -std::numeric_limits<double>::infinity();
    double rackStart = notANumber(), rackEnd = notANumber();
    double rawStart = notANumber(), rawEnd = notANumber();
    double errorStart = notANumber(), errorEnd = notANumber();
    double commandDeviation = 0.0;
    double timeAtUpper = 0.0, timeAtLower = 0.0;
    double shadowDivergenceCurrent = 0.0, shadowDivergencePrevious = 0.0;
    double shadowRateStart = notANumber(), shadowRateEnd = notANumber();
    double shadowRateAtFiveTau = notANumber();

    double mean(double integral) const { return seconds > 0.0 ? integral / seconds : notANumber(); }
};

struct Recorder {
    Runtime &runtime;
    std::ofstream &trace;
    std::vector<PhaseStats> phases;
    double time = 0.0;
    double nextTrace = 0.0;
    double previousSpeed = notANumber();
    double previousRaw = notANumber();
    bool shadowActive = false;
    double shadowAge = 0.0;
    ShadowGovernor shadowCurrent, shadowPrevious;

    Recorder(Runtime &r, std::ofstream &t) : runtime(r), trace(t) {}

    void startShadow(double targetSpeed, double speed, double raw) {
        const double error = targetSpeed * targetSpeed - speed * speed;
        const double steady = std::max(GovernorMinRate,
            std::min(GovernorMaxRate, GovernorKs * error / GovernorKd));
        shadowCurrent.rack = shadowPrevious.rack = raw;
        shadowCurrent.rate = shadowPrevious.rate = steady;
        shadowActive = true;
        shadowAge = 0.0;
    }

    void run(const std::string &name, double command, int frames, bool starter, bool measured) {
        runtime.engine->setSpeedControl(command);
        PhaseStats stats;
        stats.name = name;
        stats.measured = measured;
        stats.commanded = command;
        double phaseTime = 0.0;

        for (int frame = 0; frame < frames; ++frame) {
            runtime.simulator->m_starterMotor.m_enabled = starter;
            runtime.simulator->startFrame(1.0 / 60.0);
            while (runtime.simulator->simulateStep()) {
                const double dt = runtime.simulator->getTimestep();
                time += dt;
                phaseTime += dt;
                sample(stats, dt, phaseTime, starter);
            }
        }
        phases.push_back(stats);
    }

    void sample(PhaseStats &s, double dt, double phaseTime, bool starter) {
        Engine *engine = runtime.engine;
        const double applied = engine->getSpeedControl();
        const double control = clamp01(applied);
        const double targetRpm =
            (1.0 - control) * GovernorMinRpm + control * GovernorMaxRpm;
        const double targetSpeed = units::rpm(targetRpm);
        const double speed = engine->getSpeed();
        const double signedSpeed = engine->getCrankshaft(0)->m_body.v_theta;
        const double heldRpm = units::toRpm(speed);
        const double error = targetSpeed * targetSpeed - speed * speed;
        const double rack = engine->getFuelRack();
        const double raw = std::pow(clamp01(rack), 1.0 / GovernorGamma);
        const double rawRate = std::isfinite(previousRaw) ? (raw - previousRaw) / dt : notANumber();

        require(std::isfinite(applied) && std::isfinite(speed) && std::isfinite(signedSpeed)
                && std::isfinite(rack),
            "governor state became non-finite");

        if (s.name == "baseline" && !shadowActive) startShadow(targetSpeed, speed, raw);

        double shadowRackCurrent = notANumber(), shadowRackPrevious = notANumber();
        if (shadowActive) {
            const double prior = std::isfinite(previousSpeed) ? previousSpeed : speed;
            shadowRackCurrent = shadowCurrent.update(dt, targetSpeed, speed);
            shadowRackPrevious = shadowPrevious.update(dt, targetSpeed, prior);
            shadowAge += dt;
        }

        if (s.measured) {
            s.seconds += dt;
            s.heldTime += heldRpm * dt;
            s.signedTime += signedSpeed * dt;
            s.errorTime += error * dt;
            s.rackTime += rack * dt;
            s.heldMin = std::min(s.heldMin, heldRpm);
            s.heldMax = std::max(s.heldMax, heldRpm);
            s.errorMin = std::min(s.errorMin, error);
            s.errorMax = std::max(s.errorMax, error);
            s.rackMin = std::min(s.rackMin, rack);
            s.rackMax = std::max(s.rackMax, rack);
            if (!std::isfinite(s.rackStart)) {
                s.rackStart = rack;
                s.rawStart = raw;
                s.errorStart = error;
                s.shadowRateStart = shadowCurrent.rate;
            }
            s.rackEnd = rack;
            s.rawEnd = raw;
            s.errorEnd = error;
            s.shadowRateEnd = shadowCurrent.rate;
            s.commandDeviation = std::max(s.commandDeviation, std::abs(applied - s.commanded));
            if (raw >= 1.0 - 1.0e-12) s.timeAtUpper += dt;
            if (raw <= 1.0e-12) s.timeAtLower += dt;
            if (!std::isfinite(s.shadowRateAtFiveTau) && phaseTime >= 5.0 / GovernorKd) {
                s.shadowRateAtFiveTau = shadowCurrent.rate;
            }
            if (shadowActive && shadowAge >= ShadowSettle) {
                s.shadowDivergenceCurrent = std::max(
                    s.shadowDivergenceCurrent, std::abs(shadowRackCurrent - rack));
                s.shadowDivergencePrevious = std::max(
                    s.shadowDivergencePrevious, std::abs(shadowRackPrevious - rack));
            }
        }

        const bool transient = phaseTime <= TransientCapture
            && (s.name == "high" || s.name == "release");
        if (transient || time >= nextTrace) {
            trace << std::setprecision(12)
                << s.name << ',' << time << ',' << phaseTime << ','
                << (starter ? 1 : 0) << ','
                << runtime.simulator->m_dyno.m_rotationSpeed << ','
                << s.commanded << ',' << applied << ',' << targetRpm << ','
                << heldRpm << ',' << signedSpeed << ',' << error << ','
                << rack << ',' << raw << ',' << rawRate << ','
                << shadowRackCurrent << ',' << shadowRackPrevious << ','
                << (shadowActive ? shadowCurrent.rate : notANumber()) << ','
                << (transient ? 1 : 0) << '\n';
            if (time >= nextTrace) nextTrace += TraceInterval;
        }

        previousSpeed = speed;
        previousRaw = raw;
    }

    const PhaseStats &phase(const std::string &name) const {
        for (const PhaseStats &p : phases) if (p.name == name) return p;
        throw EvidenceFailure("missing phase " + name);
    }
};

void writePhase(std::ostream &out, const PhaseStats &p) {
    const std::string k = "GATE6B_EVIDENCE phase=" + p.name + " ";
    out << std::setprecision(10)
        << k << "seconds=" << p.seconds << '\n'
        << k << "commanded=" << p.commanded << '\n'
        << k << "max_command_deviation=" << p.commandDeviation << '\n'
        << k << "held_rpm_mean=" << p.mean(p.heldTime) << '\n'
        << k << "held_rpm_min=" << p.heldMin << '\n'
        << k << "held_rpm_max=" << p.heldMax << '\n'
        << k << "signed_crank_speed_mean_rad_s=" << p.mean(p.signedTime) << '\n'
        << k << "error_mean=" << p.mean(p.errorTime) << '\n'
        << k << "error_min=" << p.errorMin << '\n'
        << k << "error_max=" << p.errorMax << '\n'
        << k << "error_start=" << p.errorStart << '\n'
        << k << "error_end=" << p.errorEnd << '\n'
        << k << "rack_mean=" << p.mean(p.rackTime) << '\n'
        << k << "rack_min=" << p.rackMin << '\n'
        << k << "rack_max=" << p.rackMax << '\n'
        << k << "rack_start=" << p.rackStart << '\n'
        << k << "rack_end=" << p.rackEnd << '\n'
        << k << "internal_rack_start=" << p.rawStart << '\n'
        << k << "internal_rack_end=" << p.rawEnd << '\n'
        << k << "time_at_rack_upper_s=" << p.timeAtUpper << '\n'
        << k << "time_at_rack_lower_s=" << p.timeAtLower << '\n'
        << k << "shadow_rate_start=" << p.shadowRateStart << '\n'
        << k << "shadow_rate_end=" << p.shadowRateEnd << '\n'
        << k << "shadow_rate_at_5tau=" << p.shadowRateAtFiveTau << '\n'
        << k << "shadow_divergence_current_speed=" << p.shadowDivergenceCurrent << '\n'
        << k << "shadow_divergence_previous_speed=" << p.shadowDivergencePrevious << '\n';
}

std::string classify(const Recorder &recorder, double forwardSign, std::ostream &out) {
    const PhaseStats &high = recorder.phase("high");
    const PhaseStats &release = recorder.phase("release");

    bool fixtureValid = true;
    bool commandValid = true;
    double shadowDivergence = 0.0;
    for (const PhaseStats &p : recorder.phases) {
        if (!p.measured) continue;
        const double heldMean = p.mean(p.heldTime);
        if (std::abs(heldMean - HeldRpm) > HeldMeanTolerance * HeldRpm
            || p.heldMin < (1.0 - HeldExcursionTolerance) * HeldRpm
            || p.heldMax > (1.0 + HeldExcursionTolerance) * HeldRpm
            || p.mean(p.signedTime) * forwardSign <= 0.0)
        {
            fixtureValid = false;
        }
        if (p.commandDeviation > CommandTolerance) commandValid = false;
        shadowDivergence = std::max(shadowDivergence,
            std::min(p.shadowDivergenceCurrent, p.shadowDivergencePrevious));
    }
    const bool shadowValid = shadowDivergence <= ShadowTolerance;

    const double highError = high.mean(high.errorTime);
    const double releaseError = release.mean(release.errorTime);
    const bool gate6ReleaseSatisfied =
        release.mean(release.rackTime) < high.mean(high.rackTime);
    const bool releaseNegative = releaseError < -NeutralErrorFraction * std::abs(highError);
    const bool releaseRetreated = release.rackEnd < release.rackStart - ReleaseRetreat;
    const bool persistent = std::isfinite(release.shadowRateAtFiveTau)
        && release.shadowRateAtFiveTau > PersistentRateFraction * std::abs(high.shadowRateEnd)
        && release.shadowRateAtFiveTau > 1.0e-6;

    out << std::setprecision(10)
        << "GATE6B_EVIDENCE fixture_valid=" << fixtureValid << '\n'
        << "GATE6B_EVIDENCE command_valid=" << commandValid << '\n'
        << "GATE6B_EVIDENCE shadow_max_divergence=" << shadowDivergence << '\n'
        << "GATE6B_EVIDENCE shadow_valid=" << shadowValid << '\n'
        << "GATE6B_EVIDENCE gate6_release_assertion_satisfied=" << gate6ReleaseSatisfied << '\n'
        << "GATE6B_EVIDENCE release_error_negative=" << releaseNegative << '\n'
        << "GATE6B_EVIDENCE release_rack_retreated=" << releaseRetreated << '\n'
        << "GATE6B_EVIDENCE release_rate_persistent=" << persistent << '\n';

    if (!fixtureValid) return "FIXTURE_INVALID";
    if (!commandValid || !shadowValid) return "COMMAND_MAPPING";
    if (releaseNegative && releaseRetreated && gate6ReleaseSatisfied) return "RELEASE_RESPONDS";
    if (!releaseNegative) return "FIXTURE_ASSUMPTION";
    if (persistent) return "PERSISTENT_STATE";
    if (!releaseRetreated) return "RELEASE_LOGIC_OR_WINDUP";
    return "UNCLASSIFIED";
}

void governorObservability(const char *script, const char *evidencePath, const char *tracePath) {
    std::ofstream trace(tracePath);
    require(static_cast<bool>(trace), "could not open trace output");
    trace << "phase,time_s,phase_time_s,starter,dyno_rotation_speed_rad_s,"
             "commanded,applied,target_rpm,held_rpm,signed_crank_speed_rad_s,"
             "error_rad2_s2,rack,internal_rack,internal_rack_rate,"
             "shadow_rack_current_speed,shadow_rack_previous_speed,shadow_rate,"
             "transient_capture\n";

    Runtime runtime;
    runtime.load(script);
    requireNative251B(runtime);

    // Identical to the corrected Gate 6 runLoadedSequence(). Run 63 showed a
    // positive dyno speed reverses this engine; the starter defines forward.
    const double forwardSign =
        std::copysign(1.0, runtime.simulator->m_starterMotor.m_rotationSpeed);
    require(runtime.simulator->m_starterMotor.m_rotationSpeed != 0.0,
        "starter direction is undefined");
    runtime.engine->getIgnitionModule()->m_enabled = true;
    runtime.engine->resetFuelConsumption();
    runtime.simulator->m_dyno.m_enabled = true;
    runtime.simulator->m_dyno.m_hold = true;
    runtime.simulator->m_dyno.m_rotationSpeed = forwardSign * units::rpm(HeldRpm);
    runtime.simulator->m_dyno.m_maxTorque = units::torque(20000.0, units::ft_lb);

    Recorder recorder(runtime, trace);
    recorder.run("start", LowCommand, 180, true, false);
    recorder.run("settle", LowCommand, 120, false, false);
    recorder.run("baseline", LowCommand, 120, false, true);
    recorder.run("high", HighCommand, 300, false, true);
    recorder.run("release", ReleaseCommand, 300, false, true);

    std::ostringstream evidence;
    evidence << std::setprecision(10)
        << "GATE6B_EVIDENCE fixture=gate6_corrected_forward_release_to_idle\n"
        << "GATE6B_EVIDENCE simulation_frequency=" << SimulationFrequency << '\n'
        << "GATE6B_EVIDENCE timestep_s=" << runtime.simulator->getTimestep() << '\n'
        << "GATE6B_EVIDENCE dyno_rotation_speed_rad_s="
            << runtime.simulator->m_dyno.m_rotationSpeed << '\n'
        << "GATE6B_EVIDENCE starter_rotation_speed_rad_s="
            << runtime.simulator->m_starterMotor.m_rotationSpeed << '\n'
        << "GATE6B_EVIDENCE assumed_governor min_rpm=" << GovernorMinRpm
            << " max_rpm=" << GovernorMaxRpm << " k_s=" << GovernorKs
            << " k_d=" << GovernorKd << " min_v=" << GovernorMinRate
            << " max_v=" << GovernorMaxRate << " gamma=" << GovernorGamma
            << " starting_rack=" << GovernorStartingRack << '\n'
        << "GATE6B_EVIDENCE commands low=" << LowCommand << " high=" << HighCommand
            << " release=" << ReleaseCommand << '\n'
        << "GATE6B_EVIDENCE thresholds held_mean=" << HeldMeanTolerance
            << " held_excursion=" << HeldExcursionTolerance
            << " command=" << CommandTolerance << " shadow=" << ShadowTolerance
            << " neutral_error=" << NeutralErrorFraction
            << " release_retreat=" << ReleaseRetreat
            << " persistent_rate=" << PersistentRateFraction << '\n';
    for (const PhaseStats &p : recorder.phases) {
        if (p.measured) writePhase(evidence, p);
    }
    const std::string classification = classify(recorder, forwardSign, evidence);
    evidence << "GATE6B_CLASSIFICATION=" << classification << '\n';

    trace.close();
    require(static_cast<bool>(trace), "trace output was not written completely");

    std::ofstream summary(evidencePath);
    require(static_cast<bool>(summary), "could not open evidence output");
    summary << evidence.str();
    summary.close();
    require(static_cast<bool>(summary), "evidence output was not written completely");

    std::cout << evidence.str();
    runtime.finish();
}
}

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "usage: engine-sim-governor-observability"
                     " <engine.mr> <evidence.txt> <trace.csv>\n";
        return 2;
    }
    try {
        governorObservability(argv[1], argv[2], argv[3]);
    }
    catch (const EvidenceFailure &failure) {
        std::cerr << "GATE6B_FAIL classification=evidence reason=" << failure.what() << "\n";
        return 10;
    }
    catch (const std::exception &failure) {
        std::cerr << "GATE6B_FAIL classification=configuration reason=" << failure.what() << "\n";
        return 11;
    }
    return 0;
}
