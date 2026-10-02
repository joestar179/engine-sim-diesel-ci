#include "../scripting/include/compiler.h"
#include "../include/engine.h"
#include "../include/simulator.h"
#include "../include/transmission.h"
#include "../include/vehicle.h"
#include "../include/units.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
constexpr double GasConstant = 8.31446261815324;
constexpr int SimulationFrequency = 8000;

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

GasSystem::Mix airMix() {
    GasSystem::Mix mix;
    mix.p_fuel = 0.0;
    mix.p_inert = 0.75;
    mix.p_o2 = 0.25;
    return mix;
}

Vehicle *makeDefaultVehicle() {
    Vehicle::Parameters p;
    p.mass = units::mass(1597, units::kg);
    p.diffRatio = 3.42;
    p.tireRadius = units::distance(10, units::inch);
    p.dragCoefficient = 0.25;
    p.crossSectionArea = units::distance(6.0, units::foot)
        * units::distance(6.0, units::foot);
    p.rollingResistance = 2000.0;
    Vehicle *vehicle = new Vehicle;
    vehicle->initialize(p);
    return vehicle;
}

Transmission *makeDefaultTransmission() {
    const double ratios[] = { 2.97, 2.07, 1.43, 1.00, 0.84, 0.56 };
    Transmission::Parameters p;
    p.GearCount = 6;
    p.GearRatios = ratios;
    p.MaxClutchTorque = units::torque(1000.0, units::ft_lb);
    Transmission *transmission = new Transmission;
    transmission->initialize(p);
    return transmission;
}

struct Runtime {
    es_script::Compiler compiler;
    Engine *engine = nullptr;
    Simulator *simulator = nullptr;

    void load(const char *script) {
        compiler.initialize();
        require(compiler.compile(script), "configuration", "engine script did not compile");
        auto output = compiler.execute();
        engine = output.engine;
        require(engine != nullptr, "configuration", "engine script produced no engine");

        Vehicle *vehicle = output.vehicle ? output.vehicle : makeDefaultVehicle();
        Transmission *transmission = output.transmission
            ? output.transmission : makeDefaultTransmission();
        simulator = engine->createSimulator(vehicle, transmission);
        require(simulator != nullptr, "configuration", "engine did not create a simulator");
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

TurboGroup *requireAlcoTurbo(Runtime &runtime) {
    require(runtime.engine->isCompressionIgnition(),
        "configuration", "ALCO model is not using compression ignition");
    ForcedInductionSystem *system = runtime.engine->getForcedInductionSystem();
    require(system->enabled(), "configuration", "ALCO forced induction is disabled");
    require(system->groupCount() == 1,
        "configuration", "ALCO validation configuration must contain one turbo group");
    TurboGroup *group = system->group(0);
    require(group != nullptr && group->enabled(),
        "configuration", "ALCO turbo group is unavailable");
    require(group->parameters().postTurbineExhaustIndex >= 0
            && group->parameters().postTurbineExhaustIndex
                < runtime.engine->getExhaustSystemCount(),
        "configuration", "ALCO turbo has no valid post-turbine ExhaustSystem route");
    return group;
}

double chargeMoles(const TurboGroup &group) {
    const TurboGroup::Telemetry &t = group.telemetry();
    require(t.chargePlenumPressure > 0.0 && t.chargePlenumTemperature > 0.0,
        "thermodynamics", "charge-plenum state is non-physical");
    return t.chargePlenumPressure * group.parameters().chargePlenumVolume
        / (GasConstant * t.chargePlenumTemperature);
}

void validateFiniteGroupState(const TurboGroup &group) {
    const TurboGroup::Telemetry &t = group.telemetry();
    requireFinite(t.shaftSpeed, "turbo shaft speed is non-finite");
    requireFinite(t.turbineMassFlow, "turbine mass flow is non-finite");
    requireFinite(t.turbinePower, "turbine power is non-finite");
    requireFinite(t.compressorMassFlow, "compressor mass flow is non-finite");
    requireFinite(t.compressorPower, "compressor power is non-finite");
    requireFinite(t.chargePlenumPressure, "charge-plenum pressure is non-finite");
    requireFinite(t.chargePlenumTemperature, "charge-plenum temperature is non-finite");
    requireFinite(t.coolerInletTemperature, "aftercooler inlet temperature is non-finite");
    requireFinite(t.coolerOutletTemperature, "aftercooler outlet temperature is non-finite");
    require(t.shaftSpeed >= 0.0, "thermodynamics", "turbo shaft speed became negative");
    require(t.chargePlenumPressure > 0.0 && t.chargePlenumTemperature > 0.0,
        "thermodynamics", "charge-plenum state became non-physical");
}

struct CoupledObservation {
    double cylinderExhaustMoles = 0.0;
    double maximumTurbineMassFlow = 0.0;
    double maximumTurbinePower = 0.0;
    double maximumShaftSpeed = 0.0;
    double maximumCompressorMassFlow = 0.0;
    double maximumCompressorPower = 0.0;
    double maximumChargePressureChange = 0.0;
    double maximumChargeMolesChange = 0.0;
    double maximumScrollMolesChange = 0.0;
    double maximumCooling = 0.0;
    double maximumPostTurbineMismatch = 0.0;
    double maximumRack = 0.0;
    bool turbineAccelerationObserved = false;
};

CoupledObservation runFueledAlco(Runtime &runtime, int frames) {
    TurboGroup *group = requireAlcoTurbo(runtime);
    GasSystem *scroll = group->preTurbineSystem(0);
    require(scroll != nullptr, "configuration", "ALCO pre-turbine scroll is missing");

    runtime.engine->setSpeedControl(0.0);
    runtime.engine->getIgnitionModule()->m_enabled = true;
    runtime.engine->resetFuelConsumption();

    const double initialChargePressure = group->telemetry().chargePlenumPressure;
    const double initialChargeMoles = chargeMoles(*group);
    const double initialScrollMoles = scroll->n();
    double previousShaftSpeed = group->rotatingAssembly()->shaftSpeed();
    CoupledObservation observation;

    runFrames(runtime, frames, true, [&](double) {
        const TurboGroup::Telemetry &t = group->telemetry();
        validateFiniteGroupState(*group);
        requireFinite(runtime.engine->getRpm(), "engine speed is non-finite");

        for (int i = 0; i < runtime.engine->getCylinderCount(); ++i) {
            observation.cylinderExhaustMoles += std::max(
                0.0, runtime.engine->getChamber(i)->getLastTimestepExhaustFlow());
        }

        observation.maximumTurbineMassFlow = std::max(
            observation.maximumTurbineMassFlow, t.turbineMassFlow);
        observation.maximumTurbinePower = std::max(
            observation.maximumTurbinePower, t.turbinePower);
        observation.maximumShaftSpeed = std::max(
            observation.maximumShaftSpeed, t.shaftSpeed);
        observation.maximumCompressorMassFlow = std::max(
            observation.maximumCompressorMassFlow, t.compressorMassFlow);
        observation.maximumCompressorPower = std::max(
            observation.maximumCompressorPower, t.compressorPower);
        observation.maximumChargePressureChange = std::max(
            observation.maximumChargePressureChange,
            std::abs(t.chargePlenumPressure - initialChargePressure));
        observation.maximumChargeMolesChange = std::max(
            observation.maximumChargeMolesChange,
            std::abs(chargeMoles(*group) - initialChargeMoles));
        observation.maximumScrollMolesChange = std::max(
            observation.maximumScrollMolesChange,
            std::abs(scroll->n() - initialScrollMoles));
        if (t.compressorMassFlow > 0.0 && t.compressorPower > 0.0) {
            observation.maximumCooling = std::max(
                observation.maximumCooling,
                t.coolerInletTemperature - t.coolerOutletTemperature);
        }
        observation.maximumPostTurbineMismatch = std::max(
            observation.maximumPostTurbineMismatch,
            std::abs(t.postTurbinePressure
                - runtime.engine->getExhaustSystem(
                    group->parameters().postTurbineExhaustIndex)->getSystem()->pressure()));
        observation.maximumRack = std::max(
            observation.maximumRack, runtime.engine->getFuelRack());
        if (t.turbinePower > 0.0 && t.shaftSpeed > previousShaftSpeed) {
            observation.turbineAccelerationObserved = true;
        }
        previousShaftSpeed = t.shaftSpeed;
    });

    return observation;
}

void nullNaturallyAspiratedSi(const char *script) {
    Runtime runtime;
    runtime.load(script);
    Engine *engine = runtime.engine;

    require(!engine->isCompressionIgnition(),
        "configuration", "null engine is not spark ignition");
    require(!engine->getForcedInductionSystem()->enabled(),
        "routing", "null engine unexpectedly enabled forced induction");
    require(engine->getForcedInductionSystem()->groupCount() == 0,
        "routing", "null engine created turbo gas volumes");

    for (int i = 0; i < engine->getIntakeCount(); ++i) {
        require(!engine->getIntake(i)->hasForcedInductionFeed(),
            "routing", "null engine intake was rerouted through forced induction");
        require(!engine->getIntake(i)->isAirOnly(),
            "configuration", "null SI intake was converted to an air-only diesel path");
    }
    for (int i = 0; i < engine->getExhaustSystemCount(); ++i) {
        ExhaustSystem *exhaust = engine->getExhaustSystem(i);
        require(engine->getExhaustDestination(exhaust) == exhaust->getSystem(),
            "routing", "null engine exhaust no longer uses the original ExhaustSystem path");
    }

    engine->setThrottle(0.25);
    const double firstPlate = engine->getIntake(0)->getThrottlePlatePosition();
    engine->setThrottle(0.75);
    const double secondPlate = engine->getIntake(0)->getThrottlePlatePosition();
    require(firstPlate != secondPlate,
        "control", "null SI throttle no longer changes the intake restriction");
    engine->setThrottle(0.25);
    engine->getIgnitionModule()->m_enabled = true;

    double maximumRpm = 0.0;
    runFrames(runtime, 30, true, [&](double) {
        maximumRpm = std::max(maximumRpm, engine->getRpm());
        requireFinite(engine->getRpm(), "null SI engine speed is non-finite");
        for (int i = 0; i < engine->getIntakeCount(); ++i) {
            requireFinite(engine->getIntake(i)->getSystem()->pressure(),
                "null SI intake pressure is non-finite");
            require(engine->getIntake(i)->getSystem()->pressure() > 0.0,
                "thermodynamics", "null SI intake pressure became non-physical");
        }
    });
    require(maximumRpm > 0.0, "configuration", "null SI starter did not rotate the engine");

    std::cout << "GATE5_PASS mode=null-si engine=Subaru_EJ25"
        << " max_rpm=" << maximumRpm << "\n";
    runtime.finish();
}

void alcoPassiveCranking(const char *script) {
    Runtime runtime;
    runtime.load(script);
    TurboGroup *group = requireAlcoTurbo(runtime);
    require(group->rotatingAssembly()->shaftSpeed() == 0.0,
        "configuration", "ALCO turbo shaft did not start stationary");
    require(group->telemetry().compressorPressureRatio == 1.0,
        "thermodynamics", "stationary ALCO compressor began with pressure rise");

    runtime.engine->getIgnitionModule()->m_enabled = false;
    runtime.engine->resetFuelConsumption();
    double positiveCylinderIntakeMoles = 0.0;
    double maximumCompressorMassFlow = 0.0;
    double maximumRpm = 0.0;

    runFrames(runtime, 90, true, [&](double) {
        validateFiniteGroupState(*group);
        maximumRpm = std::max(maximumRpm, runtime.engine->getRpm());
        maximumCompressorMassFlow = std::max(
            maximumCompressorMassFlow, group->telemetry().compressorMassFlow);
        for (int i = 0; i < runtime.engine->getCylinderCount(); ++i) {
            positiveCylinderIntakeMoles += std::max(
                0.0, runtime.engine->getChamber(i)->getLastTimestepIntakeFlow());
        }
    });

    require(maximumRpm > 0.0, "configuration", "starter did not crank the ALCO engine");
    require(positiveCylinderIntakeMoles > 0.0,
        "routing", "fuel-free ALCO cranking moved no air into the cylinders");
    require(maximumCompressorMassFlow > 0.0,
        "routing", "fuel-free ALCO cranking moved no air through the compressor path");
    require(runtime.engine->getDirectInjectedFuelMass() == 0.0,
        "control", "fuel was injected while ignition scheduling was disabled");

    std::cout << "GATE5_PASS mode=alco-passive-cranking"
        << " max_rpm=" << maximumRpm
        << " cylinder_intake_mol=" << positiveCylinderIntakeMoles
        << " max_compressor_kg_s=" << maximumCompressorMassFlow << "\n";
    runtime.finish();
}

void alcoExhaustToScroll(const char *script) {
    Runtime runtime;
    runtime.load(script);
    TurboGroup *group = requireAlcoTurbo(runtime);
    GasSystem *scroll = group->preTurbineSystem(0);
    require(scroll != nullptr, "configuration", "ALCO pre-turbine scroll is missing");

    for (int i = 0; i < runtime.engine->getExhaustSystemCount(); ++i) {
        ExhaustSystem *exhaust = runtime.engine->getExhaustSystem(i);
        require(runtime.engine->getExhaustDestination(exhaust) == scroll,
            "routing", "ALCO cylinder runner does not target the configured scroll");
        require(runtime.engine->getExhaustDestination(exhaust) != exhaust->getSystem(),
            "routing", "ALCO cylinder runner bypasses the turbine into ExhaustSystem");
    }
    require(group->parameters().postTurbineExhaustIndex >= 0,
        "configuration", "ALCO turbo has no post-turbine ExhaustSystem route");

    const CoupledObservation observation = runFueledAlco(runtime, 180);
    require(observation.cylinderExhaustMoles > 0.0,
        "combustion", "running ALCO produced no cylinder exhaust transfer");
    require(observation.maximumScrollMolesChange > 1.0e-12,
        "routing", "ALCO pre-turbine scroll state did not respond to cylinder exhaust");
    require(observation.maximumTurbineMassFlow > 0.0,
        "routing", "gas entering the ALCO scroll did not pass through the turbine");
    require(observation.maximumPostTurbineMismatch < 1.0e-6,
        "routing", "turbine outlet state is not the original ExhaustSystem state");

    std::cout << "GATE5_PASS mode=alco-exhaust-routing"
        << " cylinder_exhaust_mol=" << observation.cylinderExhaustMoles
        << " max_turbine_kg_s=" << observation.maximumTurbineMassFlow
        << " scroll_delta_mol=" << observation.maximumScrollMolesChange << "\n";
    runtime.finish();
}

void alcoTurbineAcceleratesShaft(const char *script) {
    Runtime runtime;
    runtime.load(script);
    TurboGroup *group = requireAlcoTurbo(runtime);
    require(group->rotatingAssembly()->shaftSpeed() == 0.0,
        "configuration", "ALCO turbo shaft did not start stationary");

    const CoupledObservation observation = runFueledAlco(runtime, 180);
    require(observation.maximumTurbineMassFlow > 0.0,
        "routing", "ALCO turbine received no transferred exhaust gas");
    require(observation.maximumTurbinePower > 0.0,
        "thermodynamics", "transferred ALCO exhaust gas supplied no turbine power");
    require(observation.maximumShaftSpeed > 0.0,
        "thermodynamics", "ALCO turbine power produced no shaft motion");
    require(observation.turbineAccelerationObserved,
        "thermodynamics", "no shaft acceleration coincided with positive turbine power");

    std::cout << "GATE5_PASS mode=alco-turbine-shaft"
        << " max_turbine_W=" << observation.maximumTurbinePower
        << " max_shaft_rpm=" << units::toRpm(observation.maximumShaftSpeed) << "\n";
    runtime.finish();
}

void alcoCompressorChangesCharge(const char *script) {
    Runtime runtime;
    runtime.load(script);
    TurboGroup *group = requireAlcoTurbo(runtime);
    const CoupledObservation observation = runFueledAlco(runtime, 240);

    require(observation.maximumShaftSpeed > 0.0,
        "thermodynamics", "ALCO turbo shaft never rotated");
    require(observation.maximumCompressorMassFlow > 0.0,
        "routing", "rotating ALCO compressor transferred no gas mass");
    require(observation.maximumCompressorPower > 0.0,
        "thermodynamics", "rotating ALCO compressor consumed no shaft power");

    // Isolate the charge path after the coupled run: there is no cylinder
    // update here, and each intake begins every substep at the current plenum
    // P/T. Any subsequent plenum change therefore comes from real gas already
    // delivered through the compressor/cooler path, not cylinder suction.
    const double isolatedInitialMoles = chargeMoles(*group);
    const double isolatedInitialPressure = group->telemetry().chargePlenumPressure;
    double isolatedMassTransferred = 0.0;
    double isolatedMolesChange = 0.0;
    double isolatedPressureChange = 0.0;
    constexpr double fluidStep = 1.0 / (SimulationFrequency * 8.0);
    for (int step = 0; step < 4000; ++step) {
        const TurboGroup::Telemetry before = group->telemetry();
        for (int intake = 0; intake < runtime.engine->getIntakeCount(); ++intake) {
            runtime.engine->getIntake(intake)->getSystem()->reset(
                before.chargePlenumPressure,
                before.chargePlenumTemperature,
                airMix());
        }
        runtime.engine->processForcedInduction(fluidStep);
        validateFiniteGroupState(*group);
        const TurboGroup::Telemetry &after = group->telemetry();
        if (after.compressorMassFlow > 0.0 && after.compressorPower > 0.0) {
            isolatedMassTransferred += after.compressorMassFlow * fluidStep;
            isolatedMolesChange = std::max(
                isolatedMolesChange,
                std::abs(chargeMoles(*group) - isolatedInitialMoles));
            isolatedPressureChange = std::max(
                isolatedPressureChange,
                std::abs(after.chargePlenumPressure - isolatedInitialPressure));
        }
    }

    require(isolatedMassTransferred > 0.0,
        "routing", "isolated rotating ALCO compressor transferred no gas mass");
    require(isolatedMolesChange > 1.0e-9,
        "routing", "compressor-delivered gas did not change ALCO charge-plenum gas amount");
    require(isolatedPressureChange > 1.0e-4,
        "thermodynamics", "ALCO charge pressure did not emerge from compressor-delivered gas");

    std::cout << "GATE5_PASS mode=alco-compressor-charge"
        << " max_compressor_kg_s=" << observation.maximumCompressorMassFlow
        << " max_compressor_W=" << observation.maximumCompressorPower
        << " isolated_compressor_kg=" << isolatedMassTransferred
        << " charge_delta_mol=" << isolatedMolesChange
        << " charge_delta_Pa=" << isolatedPressureChange << "\n";
    runtime.finish();
}

void alcoFuelControlSeparation(const char *script) {
    Runtime runtime;
    runtime.load(script);
    TurboGroup *group = requireAlcoTurbo(runtime);

    runtime.engine->setFuelRack(0.37);
    const double rackBefore = runtime.engine->getFuelRack();
    const double injectedBefore = runtime.engine->getDirectInjectedFuelMass();
    const double burnedBefore = runtime.engine->getDirectBurnedFuelMass();
    runtime.engine->processForcedInduction(1.0 / (SimulationFrequency * 8.0));

    require(runtime.engine->getFuelRack() == rackBefore,
        "control", "forced-induction processing changed the diesel fuel rack");
    require(runtime.engine->getDirectInjectedFuelMass() == injectedBefore,
        "control", "forced-induction processing injected fuel");
    require(runtime.engine->getDirectBurnedFuelMass() == burnedBefore,
        "control", "forced-induction processing burned fuel");
    require(!group->parameters().throttleEnabled,
        "configuration", "ALCO turbo configuration unexpectedly enabled a charge throttle");

    runtime.engine->setSpeedControl(0.0);
    runtime.engine->getIgnitionModule()->m_enabled = true;
    runtime.engine->resetFuelConsumption();
    double maximumRack = 0.0;
    runFrames(runtime, 120, true, [&](double) {
        maximumRack = std::max(maximumRack, runtime.engine->getFuelRack());
        validateFiniteGroupState(*group);
    });
    require(maximumRack > 0.0,
        "control", "ALCO governor did not issue a fuel-rack command");
    require(runtime.engine->getDirectInjectedFuelMass() > 0.0,
        "combustion", "active ALCO fuel control scheduled no direct injection");

    std::cout << "GATE5_PASS mode=alco-control-separation"
        << " fi_only_rack=" << rackBefore
        << " governor_max_rack=" << maximumRack
        << " injected_g=" << runtime.engine->getDirectInjectedFuelMass() * 1000.0 << "\n";
    runtime.finish();
}

void alcoAftercoolerActualCharge(const char *script) {
    Runtime runtime;
    runtime.load(script);
    TurboGroup *group = requireAlcoTurbo(runtime);
    require(group->parameters().chargeAirCoolerEnabled,
        "configuration", "ALCO aftercooler is disabled");
    require(group->parameters().aftercoolerEffectiveness > 0.0,
        "configuration", "ALCO aftercooler has no configured effectiveness");

    const CoupledObservation observation = runFueledAlco(runtime, 240);
    require(observation.maximumCompressorMassFlow > 0.0,
        "routing", "no actual compressor gas reached the ALCO charge path");
    require(observation.maximumCompressorPower > 0.0,
        "thermodynamics", "ALCO compressor did not heat a powered charge stream");
    require(observation.maximumCooling > 0.0,
        "thermodynamics", "enabled ALCO aftercooler removed no heat from flowing charge gas");

    std::cout << "GATE5_PASS mode=alco-aftercooler"
        << " max_cooling_K=" << observation.maximumCooling
        << " max_compressor_kg_s=" << observation.maximumCompressorMassFlow << "\n";
    runtime.finish();
}
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "usage: engine-sim-integration-validation <mode> <engine.mr>\n";
        return 2;
    }

    const std::string mode = argv[1];
    try {
        if (mode == "null-si") nullNaturallyAspiratedSi(argv[2]);
        else if (mode == "alco-passive-cranking") alcoPassiveCranking(argv[2]);
        else if (mode == "alco-exhaust-routing") alcoExhaustToScroll(argv[2]);
        else if (mode == "alco-turbine-shaft") alcoTurbineAcceleratesShaft(argv[2]);
        else if (mode == "alco-compressor-charge") alcoCompressorChangesCharge(argv[2]);
        else if (mode == "alco-control-separation") alcoFuelControlSeparation(argv[2]);
        else if (mode == "alco-aftercooler") alcoAftercoolerActualCharge(argv[2]);
        else throw ValidationFailure("configuration", "unknown validation mode");
    }
    catch (const ValidationFailure &failure) {
        std::cerr << "GATE5_FAIL classification=" << failure.classification()
            << " mode=" << mode << " reason=" << failure.what() << "\n";
        return 10;
    }
    catch (const std::exception &failure) {
        std::cerr << "GATE5_FAIL classification=configuration mode=" << mode
            << " reason=" << failure.what() << "\n";
        return 11;
    }

    return 0;
}
