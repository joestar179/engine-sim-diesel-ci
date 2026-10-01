// Full-load dyno sweep (diagnostic tool, no assertions).
//
// Uses only the upstream Engine Simulator API (v0.1.11a), so the same source
// builds against the unmodified upstream tree and against this overlay; the
// two builds can then be compared on the same script.
//
// For each speed: compile the script, crank, set the speed control (throttle
// for spark-ignition engines, governor command for diesels), hold the speed
// with the dynamometer, settle, then average torque, power and fuel use.
//
// usage: engine-sim-dyno-sweep <script.mr> <rpm,rpm,...>
//            [--throttle 1.0] [--settle 6] [--measure 3] [--frequency N]
//            overlay build only: [--real-gas 0|1] [--enthalpy-flow 0|1]

#include "../scripting/include/compiler.h"
#include "../include/engine.h"
#include "../include/simulator.h"
#include "../include/transmission.h"
#include "../include/vehicle.h"
#include "../include/units.h"
#ifdef ENGINE_SIM_OVERLAY
#include "../include/gas_system.h"
#include "../include/combustion_chamber.h"
#endif

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

namespace {
void advance(Simulator *sim, double seconds, int frequency,
             double *torqueSum = nullptr, double *powerSum = nullptr, long long *samples = nullptr)
{
    const long long steps = static_cast<long long>(seconds * frequency);
    long long done = 0;
    int16_t sink[4096];
    while (done < steps) {
        sim->startFrame(1.0 / 60.0);
        while (done < steps && sim->simulateStep()) {
            ++done;
            if (torqueSum != nullptr) {
                *torqueSum += sim->getFilteredDynoTorque();
                *powerSum += sim->getDynoPower();
                ++*samples;
            }
        }
        sim->endFrame();
        while (sim->readAudioOutput(4096, sink) > 0) {}
    }
}
}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::fprintf(stderr, "usage: engine-sim-dyno-sweep <script.mr> <rpm,rpm,...> [--throttle T] [--settle S] [--measure S] [--frequency N]\n");
        return 2;
    }
    std::vector<double> speeds;
    {
        std::stringstream ss(argv[2]);
        std::string item;
        while (std::getline(ss, item, ',')) speeds.push_back(std::atof(item.c_str()));
    }
    double throttle = 1.0, settle = 6.0, measure = 3.0;
    int frequency = 0;
    for (int i = 3; i + 1 < argc; i += 2) {
        const std::string a = argv[i];
        if (a == "--throttle") throttle = std::atof(argv[i + 1]);
        else if (a == "--settle") settle = std::atof(argv[i + 1]);
        else if (a == "--measure") measure = std::atof(argv[i + 1]);
        else if (a == "--frequency") frequency = std::atoi(argv[i + 1]);
#ifdef ENGINE_SIM_OVERLAY
        // Overlay-only diagnostic switches (value 0 = upstream behaviour).
        else if (a == "--real-gas") gas_vibration::enabled = std::atoi(argv[i + 1]) != 0;
        else if (a == "--enthalpy-flow") gas_vibration::enthalpyFlow = std::atoi(argv[i + 1]) != 0;
        else if (a == "--unified-heat") combustion_physics::unifiedHeatTransfer = std::atoi(argv[i + 1]) != 0;
        else if (a == "--flame-expansion") combustion_physics::flameExpansion = std::atoi(argv[i + 1]) != 0;
#endif
    }

    std::printf("rpm,torque_Nm,power_kW,fuel_g_s,bsfc_g_kWh\n");
    for (double rpm : speeds) {
        es_script::Compiler compiler;
        compiler.initialize();
        if (!compiler.compile(argv[1])) { std::fprintf(stderr, "compile failed\n"); return 10; }
        auto out = compiler.execute();
        Engine *engine = out.engine;
        if (engine == nullptr) { std::fprintf(stderr, "no engine\n"); return 11; }

        Vehicle *vehicle = out.vehicle;
        if (vehicle == nullptr) {
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
        if (transmission == nullptr) {
            static const double ratios[] = { 2.97, 2.07, 1.43, 1.00, 0.84, 0.56 };
            Transmission::Parameters p;
            p.GearCount = 6;
            p.GearRatios = ratios;
            p.MaxClutchTorque = units::torque(1000.0, units::ft_lb);
            transmission = new Transmission;
            transmission->initialize(p);
        }

        Simulator *sim = engine->createSimulator(vehicle, transmission);
        const int f = frequency > 0 ? frequency : static_cast<int>(engine->getSimulationFrequency());
        sim->setSimulationFrequency(f);
        engine->getIgnitionModule()->m_enabled = true;
        engine->setSpeedControl(0.0);

        // Crank, then run at the requested speed control with the dyno holding.
        sim->m_starterMotor.m_enabled = true;
        advance(sim, 2.0, f);
        sim->m_starterMotor.m_enabled = false;
        engine->setSpeedControl(throttle);
        sim->m_dyno.m_rotationSpeed = units::rpm(rpm);
        sim->m_dyno.m_maxTorque = units::torque(20000.0, units::ft_lb);
        sim->m_dyno.m_hold = true;
        sim->m_dyno.m_enabled = true;
        advance(sim, settle, f);

        const double fuel0 = engine->getTotalFuelMassConsumed();
        double torque = 0.0, power = 0.0;
        long long n = 0;
        advance(sim, measure, f, &torque, &power, &n);
        const double fuelRate = (engine->getTotalFuelMassConsumed() - fuel0) / measure;   // kg/s
        torque /= std::max(1LL, n);
        power /= std::max(1LL, n);
        const double kW = std::abs(power) / 1000.0;
        std::printf("%.0f,%.2f,%.3f,%.4f,%.1f\n", rpm, std::abs(torque), kW, fuelRate * 1000.0,
            kW > 0.0 ? fuelRate * 1000.0 * 3600.0 / kW : 0.0);
        std::fflush(stdout);

        sim->destroy();
        compiler.destroy();
    }
    return 0;
}
