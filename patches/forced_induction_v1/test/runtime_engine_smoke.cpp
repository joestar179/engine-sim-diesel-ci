#include "../scripting/include/compiler.h"
#include "../include/engine.h"
#include "../include/simulator.h"
#include "../include/transmission.h"
#include "../include/vehicle.h"
#include "../include/units.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>

namespace {
Vehicle *makeDefaultVehicle() {
    Vehicle::Parameters p;
    p.mass = units::mass(1597, units::kg);
    p.diffRatio = 3.42;
    p.tireRadius = units::distance(10, units::inch);
    p.dragCoefficient = 0.25;
    p.crossSectionArea = units::distance(6.0, units::foot) * units::distance(6.0, units::foot);
    p.rollingResistance = 2000.0;
    Vehicle *v = new Vehicle;
    v->initialize(p);
    return v;
}

Transmission *makeDefaultTransmission() {
    const double ratios[] = { 2.97, 2.07, 1.43, 1.00, 0.84, 0.56 };
    Transmission::Parameters p;
    p.GearCount = 6;
    p.GearRatios = ratios;
    p.MaxClutchTorque = units::torque(1000.0, units::ft_lb);
    Transmission *t = new Transmission;
    t->initialize(p);
    return t;
}
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "usage: engine-sim-runtime-smoke <alco_main.mr>\n";
        return 2;
    }

    es_script::Compiler compiler;
    compiler.initialize();
    if (!compiler.compile(argv[1])) {
        compiler.destroy();
        std::cerr << "runtime smoke: script compile failed\n";
        return 10;
    }

    auto output = compiler.execute();
    Engine *engine = output.engine;
    if (engine == nullptr || !engine->isCompressionIgnition()) {
        compiler.destroy();
        std::cerr << "runtime smoke: ALCO engine missing or CI disabled\n";
        return 11;
    }

    Vehicle *vehicle = output.vehicle ? output.vehicle : makeDefaultVehicle();
    Transmission *transmission = output.transmission ? output.transmission : makeDefaultTransmission();

    Simulator *sim = engine->createSimulator(vehicle, transmission);
    sim->setSimulationFrequency(8000);
    engine->setSpeedControl(0.0); // 400-rpm idle setpoint

    double maxRpm = 0.0;
    double maxRack = 0.0;
    double maxTurboRpm = 0.0;
    double maxTurbinePower = 0.0;
    double maxTurbinePressureRatio = 1.0;
    double maxTurbineInletPressure = 0.0;
    double maxCompressorPressureRatio = 1.0;
    double releasedStartRpm = 0.0;
    double finalRpm = 0.0;

    // Keep total simulation samples below the synthesizer input-buffer size;
    // do not call endFrame(), so headless smoke never enters the audio render path.
    engine->getIgnitionModule()->m_enabled = true;
    constexpr int frames = 180; // 3 seconds at 60 Hz
    constexpr int releaseFrame = 105; // 1.75 seconds starter-on
    for (int frame = 0; frame < frames; ++frame) {
        sim->m_starterMotor.m_enabled = frame < releaseFrame;
        if (frame == releaseFrame) releasedStartRpm = engine->getRpm();

        sim->startFrame(1.0 / 60.0);
        while (sim->simulateStep()) {
            maxRpm = std::max(maxRpm, engine->getRpm());
            maxRack = std::max(maxRack, engine->getFuelRack());
            maxTurboRpm = std::max(maxTurboRpm, units::toRpm(engine->getTurboSpeed()));
            maxTurbinePower = std::max(maxTurbinePower, engine->getTurbinePower());
            maxTurbinePressureRatio = std::max(
                maxTurbinePressureRatio,
                engine->getTurbinePressureRatio());
            maxTurbineInletPressure = std::max(
                maxTurbineInletPressure,
                engine->getTurbineInletPressure());
            maxCompressorPressureRatio = std::max(
                maxCompressorPressureRatio,
                engine->getCompressorPressureRatio());
        }
    }
    finalRpm = engine->getRpm();

    const TurboGroup *primaryTurbo = engine->getForcedInductionSystem()->group(0);
    const double finalPreTurbineTemperature =
        primaryTurbo != nullptr && !primaryTurbo->telemetry().preTurbineTemperature.empty()
        ? primaryTurbo->telemetry().preTurbineTemperature.front()
        : 0.0;

    std::cout
        << "runtime smoke telemetry"
        << " | max_rpm=" << maxRpm
        << " | rpm_at_starter_release=" << releasedStartRpm
        << " | final_rpm=" << finalRpm
        << " | max_rack=" << maxRack
        << " | max_turbo_rpm=" << maxTurboRpm
        << " | injected_fuel_g=" << engine->getDirectInjectedFuelMass() * 1000.0
        << " | burned_fuel_g=" << engine->getDirectBurnedFuelMass() * 1000.0
        << " | max_ci_temp_K=" << engine->getMaxCompressionIgnitionTemperature()
        << " | max_ci_pressure_MPa=" << engine->getMaxCompressionIgnitionPressure() / 1.0e6
        << " | max_compressor_pr=" << maxCompressorPressureRatio
        << " | final_compressor_pr=" << engine->getCompressorPressureRatio()
        << " | final_compressor_power_W=" << engine->getCompressorPower()
        << " | max_turbine_power_W=" << maxTurbinePower
        << " | max_turbine_pr=" << maxTurbinePressureRatio
        << " | max_turbine_inlet_kPa=" << maxTurbineInletPressure / 1000.0
        << " | final_turbine_inlet_kPa=" << engine->getTurbineInletPressure() / 1000.0
        << " | final_turbine_outlet_kPa=" << engine->getTurbineOutletPressure() / 1000.0
        << " | final_preturbine_pressure_kPa=" << engine->getTurbineInletPressure() / 1000.0
        << " | final_postturbine_pressure_kPa=" << engine->getExhaustSystem(0)->getSystem()->pressure() / 1000.0
        << " | final_preturbine_temp_K=" << finalPreTurbineTemperature
        << " | final_postturbine_temp_K=" << engine->getExhaustSystem(0)->getSystem()->temperature()
        << "\n";

    // These are integration gates, not final ALCO calibration targets.
    if (maxRack < 0.199) {
        std::cerr << "runtime smoke: governor never commanded starting rack\n";
        return 20;
    }
    if (maxRpm < 120.0) {
        std::cerr << "runtime smoke: starter failed to rotate engine meaningfully\n";
        return 21;
    }
    if (releasedStartRpm < 100.0) {
        std::cerr << "runtime smoke: engine was not rotating at starter release\n";
        return 22;
    }
    if (finalRpm < 120.0) {
        std::cerr << "runtime smoke: engine collapsed immediately after starter release\n";
        return 23;
    }

    // A running diesel should create some exhaust energy and therefore some
    // turbo shaft motion. The threshold is intentionally tiny at this stage.
    if (maxTurboRpm <= 0.1) {
        std::cerr << "runtime smoke: turbo never received exhaust energy\n";
        return 24;
    }
    if (maxTurbinePressureRatio <= 1.02 || maxTurbinePower <= 100.0) {
        std::cerr << "runtime smoke: in-series turbine did not develop material pressure work\n";
        return 25;
    }
    if (maxTurbineInletPressure <= units::pressure(1.03, units::atm)) {
        std::cerr << "runtime smoke: pre-turbine manifold never developed backpressure\n";
        return 26;
    }

    std::cout << "runtime smoke: PASS\n";

    // The short-lived CI process exits immediately after this audit. Avoid
    // double-destroying objects held by the script compiler's output graph.
    (void)sim;
    compiler.destroy();
    return 0;
}

