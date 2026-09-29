#include "../scripting/include/compiler.h"
#include "../include/engine.h"
#include "../include/simulator.h"
#include "../include/transmission.h"
#include "../include/vehicle.h"
#include "../include/units.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {
constexpr int SimulationFrequency = 8000;
constexpr double GasConstant = 8.31446261815324;

class ValidationFailure : public std::runtime_error {
public:
    ValidationFailure(const std::string &classification, const std::string &message)
        : std::runtime_error(message), m_classification(classification) {}

    const std::string &classification() const { return m_classification; }

private:
    std::string m_classification;
};

void require(bool condition, const char *classification, const char *message) {
    if (!condition) throw ValidationFailure(classification, message);
}

void requireFinite(double value, const char *message) {
    require(std::isfinite(value), "thermodynamics", message);
}

std::string runtimeErrorDetails() {
    std::ifstream errorLog("error_log.log");
    std::ostringstream details;
    details << errorLog.rdbuf();
    return details.str();
}

struct Runtime {
    es_script::Compiler compiler;
    Engine *engine = nullptr;
    Simulator *simulator = nullptr;

    void load(const char *script) {
        compiler.initialize();
        if (!compiler.compile(script)) {
            std::ifstream errorLog("error_log.log");
            std::ostringstream details;
            details << errorLog.rdbuf();
            throw ValidationFailure(
                "configuration",
                "reference script did not compile: " + details.str());
        }
        auto output = compiler.execute();
        engine = output.engine;
        if (engine == nullptr) {
            throw ValidationFailure(
                "configuration",
                "reference script produced no engine: "
                    + runtimeErrorDetails());
        }
        require(output.vehicle != nullptr && output.transmission != nullptr,
            "configuration", "reference load model is missing");
        simulator = engine->createSimulator(output.vehicle, output.transmission);
        require(simulator != nullptr, "configuration",
            "reference script did not create a simulator");
        simulator->setSimulationFrequency(SimulationFrequency);
    }

    void finish() { compiler.destroy(); }
};

void runFrames(
    Runtime &runtime,
    int frameCount,
    bool starterEnabled,
    const std::function<void(double)> &observe)
{
    for (int frame = 0; frame < frameCount; ++frame) {
        runtime.simulator->m_starterMotor.m_enabled = starterEnabled;
        runtime.simulator->startFrame(1.0 / 60.0);
        while (runtime.simulator->simulateStep()) {
            observe(runtime.simulator->getTimestep());
        }
    }
}

TurboGroup *requireNative251B(Runtime &runtime) {
    require(runtime.engine->getCylinderCount() == 16,
        "configuration", "reference engine is not the 16-cylinder model");
    require(runtime.engine->isCompressionIgnition(),
        "configuration", "16-251B is not using native compression ignition");
    require(runtime.engine->getIntakeCount() == 1,
        "configuration", "16-251B must use its documented common intake");
    require(runtime.engine->getExhaustSystemCount() == 4,
        "configuration", "16-251B must expose four pulse branches");

    ForcedInductionSystem *system = runtime.engine->getForcedInductionSystem();
    require(system->enabled() && system->groupCount() == 1,
        "configuration", "16-251B must use one native TurboGroup");
    TurboGroup *group = system->group(0);
    require(group != nullptr && group->enabled(),
        "configuration", "16-251B TurboGroup is unavailable");
    require(group->telemetry().preTurbinePressure.size() == 4,
        "routing", "16-251B TurboGroup does not retain four scroll states");
    require(group->parameters().postTurbineExhaustIndex >= 0
            && group->parameters().postTurbineExhaustIndex
                < runtime.engine->getExhaustSystemCount(),
        "routing", "16-251B has no valid post-turbine ExhaustSystem");

    std::set<GasSystem *> routedScrolls;
    for (int i = 0; i < runtime.engine->getExhaustSystemCount(); ++i) {
        ExhaustSystem *exhaust = runtime.engine->getExhaustSystem(i);
        GasSystem *destination = runtime.engine->getExhaustDestination(exhaust);
        require(destination != exhaust->getSystem(), "routing",
            "16-251B pulse branch bypasses the turbine");
        routedScrolls.insert(destination);
    }
    require(routedScrolls.size() == 4,
        "routing", "16-251B pulse branches do not route to four distinct scrolls");

    return group;
}

double chargeMoles(const TurboGroup &group) {
    const TurboGroup::Telemetry &t = group.telemetry();
    require(t.chargePlenumPressure > 0.0 && t.chargePlenumTemperature > 0.0,
        "thermodynamics", "charge-plenum state is non-physical");
    return t.chargePlenumPressure * group.parameters().chargePlenumVolume
        / (GasConstant * t.chargePlenumTemperature);
}

void validateState(Runtime &runtime, TurboGroup &group) {
    const TurboGroup::Telemetry &t = group.telemetry();
    requireFinite(runtime.engine->getRpm(), "engine speed is non-finite");
    requireFinite(runtime.engine->getFuelRack(), "fuel rack is non-finite");
    requireFinite(t.turbineMassFlow, "turbine mass flow is non-finite");
    requireFinite(t.turbinePower, "turbine power is non-finite");
    requireFinite(t.shaftSpeed, "shaft speed is non-finite");
    requireFinite(t.compressorMassFlow, "compressor mass flow is non-finite");
    requireFinite(t.compressorPower, "compressor power is non-finite");
    requireFinite(t.chargePlenumPressure, "charge pressure is non-finite");
    requireFinite(t.chargePlenumTemperature, "charge temperature is non-finite");
    require(t.shaftSpeed >= 0.0
            && t.shaftSpeed <= group.parameters().maxSpeed * 1.000001,
        "thermodynamics", "turbo shaft exceeded its physical bounds");
    require(t.chargePlenumPressure > 0.0 && t.chargePlenumTemperature > 0.0,
        "thermodynamics", "charge-plenum state became non-physical");
}

struct Window {
    double seconds = 0.0;
    double rackTime = 0.0;
    double rpmTime = 0.0;
    double exhaustMoles = 0.0;
    double intakeMoles = 0.0;
    double turbinePowerTime = 0.0;
    double compressorFlowTime = 0.0;
    double compressorPowerTime = 0.0;
    double shaftSpeedTime = 0.0;
    double chargePressureTime = 0.0;
    double chargeMolesTime = 0.0;
    double maximumShaftSpeed = 0.0;
    double maximumTurbinePower = 0.0;
    double finalShaftSpeed = 0.0;
    double finalChargePressure = 0.0;
    double injectedFuel = 0.0;
    double burnedFuel = 0.0;
};

Window observeWindow(Runtime &runtime, TurboGroup &group, int frames, bool starter) {
    Window result;
    const double injectedBefore = runtime.engine->getDirectInjectedFuelMass();
    const double burnedBefore = runtime.engine->getDirectBurnedFuelMass();

    runFrames(runtime, frames, starter, [&](double dt) {
        validateState(runtime, group);
        const TurboGroup::Telemetry &t = group.telemetry();
        result.seconds += dt;
        result.rackTime += runtime.engine->getFuelRack() * dt;
        result.rpmTime += runtime.engine->getRpm() * dt;
        result.turbinePowerTime += t.turbinePower * dt;
        result.compressorFlowTime += t.compressorMassFlow * dt;
        result.compressorPowerTime += t.compressorPower * dt;
        result.shaftSpeedTime += t.shaftSpeed * dt;
        result.chargePressureTime += t.chargePlenumPressure * dt;
        result.chargeMolesTime += chargeMoles(group) * dt;
        result.maximumShaftSpeed = std::max(result.maximumShaftSpeed, t.shaftSpeed);
        result.maximumTurbinePower = std::max(result.maximumTurbinePower, t.turbinePower);
        result.finalShaftSpeed = t.shaftSpeed;
        result.finalChargePressure = t.chargePlenumPressure;

        for (int i = 0; i < runtime.engine->getCylinderCount(); ++i) {
            result.exhaustMoles += std::max(
                0.0, runtime.engine->getChamber(i)->getLastTimestepExhaustFlow());
            result.intakeMoles += std::max(
                0.0, runtime.engine->getChamber(i)->getLastTimestepIntakeFlow());
        }
    });

    result.injectedFuel =
        runtime.engine->getDirectInjectedFuelMass() - injectedBefore;
    result.burnedFuel =
        runtime.engine->getDirectBurnedFuelMass() - burnedBefore;
    require(result.seconds > 0.0, "configuration",
        "loaded-transient observation window was empty");
    return result;
}

double mean(double timeIntegral, const Window &window) {
    return timeIntegral / window.seconds;
}

double rate(double quantity, const Window &window) {
    return quantity / window.seconds;
}

void stockSiCompatibility(const char *script) {
    Runtime runtime;
    runtime.load(script);
    require(!runtime.engine->isCompressionIgnition(), "configuration",
        "official v0.1.14a null reference is not spark ignition");
    require(!runtime.engine->getForcedInductionSystem()->enabled(), "routing",
        "official v0.1.14a null reference unexpectedly enabled forced induction");
    for (int i = 0; i < runtime.engine->getExhaustSystemCount(); ++i) {
        ExhaustSystem *exhaust = runtime.engine->getExhaustSystem(i);
        require(runtime.engine->getExhaustDestination(exhaust) == exhaust->getSystem(),
            "routing", "turbo-disabled SI exhaust path is not the original path");
    }

    std::cout << "GATE6_PASS mode=v014a-stock-si-load"
        << " cylinders=" << runtime.engine->getCylinderCount()
        << " exhaust_systems=" << runtime.engine->getExhaustSystemCount()
        << " forced_induction=disabled\n";
    runtime.finish();
}

struct Sequence {
    Window baseline;
    Window response;
    Window release;
};

Sequence runLoadedSequence(Runtime &runtime, bool includeRelease) {
    TurboGroup *group = requireNative251B(runtime);
    runtime.engine->getIgnitionModule()->m_enabled = true;
    runtime.engine->resetFuelConsumption();

    // External dynamometer hold is a test-bench load, not boost or governor
    // control. Holding 600 rpm lets a notch command change fuel/exhaust energy
    // without engine-speed change obscuring the causal chain. The hold uses the
    // starter's sign: Gate 6B Run 63 showed a positive speed reverses the engine.
    require(runtime.simulator->m_starterMotor.m_rotationSpeed != 0.0,
        "configuration", "starter direction is undefined");
    runtime.simulator->m_dyno.m_enabled = true;
    runtime.simulator->m_dyno.m_hold = true;
    runtime.simulator->m_dyno.m_rotationSpeed = std::copysign(
        units::rpm(600), runtime.simulator->m_starterMotor.m_rotationSpeed);
    runtime.simulator->m_dyno.m_maxTorque =
        units::torque(20000.0, units::ft_lb);

    constexpr double LowCommand = (600.0 - 400.0) / (1000.0 - 400.0);
    // Release to idle: a target equal to the held speed leaves an isochronous
    // governor with zero error (Gate 6B Run 63), so the rack could never move.
    constexpr double IdleCommand = 0.0;
    runtime.engine->setSpeedControl(LowCommand);

    // Start and settle at the held speed before measuring the low-command state.
    observeWindow(runtime, *group, 180, true);
    observeWindow(runtime, *group, 120, false);

    Sequence sequence;
    sequence.baseline = observeWindow(runtime, *group, 120, false);

    runtime.engine->setSpeedControl(1.0);
    sequence.response = observeWindow(runtime, *group, 300, false);

    if (includeRelease) {
        runtime.engine->setSpeedControl(IdleCommand);
        sequence.release = observeWindow(runtime, *group, 300, false);
    }
    return sequence;
}

void loadedCausalChain(const char *script) {
    Runtime runtime;
    runtime.load(script);
    TurboGroup *group = requireNative251B(runtime);
    const Sequence sequence = runLoadedSequence(runtime, false);
    const Window &low = sequence.baseline;
    const Window &high = sequence.response;

    require(mean(high.rackTime, high) > mean(low.rackTime, low),
        "control", "higher notch command did not increase fuel rack");
    require(rate(high.injectedFuel, high) > rate(low.injectedFuel, low),
        "combustion", "higher rack did not increase direct injection");
    require(rate(high.burnedFuel, high) > rate(low.burnedFuel, low),
        "combustion", "higher direct injection did not increase burned fuel");
    require(rate(high.exhaustMoles, high) > rate(low.exhaustMoles, low),
        "routing", "higher combustion did not increase cylinder exhaust transfer");
    require(mean(high.turbinePowerTime, high) > mean(low.turbinePowerTime, low),
        "thermodynamics", "higher exhaust energy did not increase turbine power");
    require(high.maximumShaftSpeed > low.maximumShaftSpeed,
        "thermodynamics", "higher turbine power did not accelerate the turbo shaft");
    require(mean(high.compressorFlowTime, high)
            > mean(low.compressorFlowTime, low),
        "routing", "faster shaft did not increase compressor mass transfer");
    require(mean(high.compressorPowerTime, high)
            > mean(low.compressorPowerTime, low),
        "thermodynamics", "compressor did not consume additional shaft power");
    require(mean(high.chargeMolesTime, high) > mean(low.chargeMolesTime, low),
        "routing", "compressor delivery did not increase charge-plenum gas");
    require(mean(high.chargePressureTime, high)
            > mean(low.chargePressureTime, low),
        "thermodynamics", "charge pressure did not emerge from added charge gas");
    require(rate(high.intakeMoles, high) > rate(low.intakeMoles, low),
        "routing", "changed charge state did not increase cylinder air delivery");

    const int postIndex = group->parameters().postTurbineExhaustIndex;
    require(std::abs(group->telemetry().postTurbinePressure
            - runtime.engine->getExhaustSystem(postIndex)->getSystem()->pressure())
            < 1.0e-6,
        "routing", "original ExhaustSystem is not the post-turbine gas state");

    std::cout << "GATE6_PASS mode=alco-251b-causal"
        << " low_rack=" << mean(low.rackTime, low)
        << " high_rack=" << mean(high.rackTime, high)
        << " low_fuel_g_s=" << 1000.0 * rate(low.injectedFuel, low)
        << " high_fuel_g_s=" << 1000.0 * rate(high.injectedFuel, high)
        << " low_turbine_W=" << mean(low.turbinePowerTime, low)
        << " high_turbine_W=" << mean(high.turbinePowerTime, high)
        << " low_shaft_rpm=" << units::toRpm(low.maximumShaftSpeed)
        << " high_shaft_rpm=" << units::toRpm(high.maximumShaftSpeed)
        << " low_charge_Pa=" << mean(low.chargePressureTime, low)
        << " high_charge_Pa=" << mean(high.chargePressureTime, high)
        << "\n";
    runtime.finish();
}

void stableRelease(const char *script) {
    Runtime runtime;
    runtime.load(script);
    TurboGroup *group = requireNative251B(runtime);
    const Sequence sequence = runLoadedSequence(runtime, true);
    const Window &high = sequence.response;
    const Window &release = sequence.release;

    require(mean(release.rackTime, release) < mean(high.rackTime, high),
        "control", "fuel rack did not retreat after notch release");
    require(rate(release.injectedFuel, release) < rate(high.injectedFuel, high),
        "combustion", "injection did not fall after rack release");
    require(mean(release.turbinePowerTime, release)
            < mean(high.turbinePowerTime, high),
        "thermodynamics", "turbine power did not fall after fueling release");
    require(release.finalShaftSpeed <= high.maximumShaftSpeed * 1.000001,
        "thermodynamics", "turbo shaft ran away after fueling release");
    require(release.finalChargePressure > 0.0,
        "thermodynamics", "charge pressure became non-physical after release");
    require(group->telemetry().shaftSpeed
            <= group->parameters().maxSpeed * 1.000001,
        "thermodynamics", "turbo shaft exceeded configured maximum speed");

    std::cout << "GATE6_PASS mode=alco-251b-release"
        << " high_rack=" << mean(high.rackTime, high)
        << " release_rack=" << mean(release.rackTime, release)
        << " high_turbine_W=" << mean(high.turbinePowerTime, high)
        << " release_turbine_W=" << mean(release.turbinePowerTime, release)
        << " peak_shaft_rpm=" << units::toRpm(high.maximumShaftSpeed)
        << " release_shaft_rpm=" << units::toRpm(release.finalShaftSpeed)
        << "\n";
    runtime.finish();
}
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr
            << "usage: engine-sim-loaded-transient-validation <mode> <engine.mr>\n";
        return 2;
    }

    const std::string mode = argv[1];
    try {
        if (mode == "v014a-stock-si-load") stockSiCompatibility(argv[2]);
        else if (mode == "alco-251b-causal") loadedCausalChain(argv[2]);
        else if (mode == "alco-251b-release") stableRelease(argv[2]);
        else throw ValidationFailure("configuration", "unknown validation mode");
    }
    catch (const ValidationFailure &failure) {
        std::cerr << "GATE6_FAIL classification=" << failure.classification()
            << " mode=" << mode << " reason=" << failure.what() << "\n";
        return 10;
    }
    catch (const std::exception &failure) {
        std::cerr << "GATE6_FAIL classification=configuration mode=" << mode
            << " reason=" << failure.what() << "\n";
        return 11;
    }

    return 0;
}
