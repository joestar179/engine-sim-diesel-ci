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
//            [--torque Nm]  part load: bisect the speed control until the
//                           held-speed torque equals Nm (steady-state test
//                           cycles such as 40 CFR 1054 Appendix B / ISO 8178)
//            [--map kPa]    bisect the speed control until the mean intake
//                           plenum pressure equals kPa (measured-MAP boundary)
//            [--fuel g/s]   bisect the speed control until the metered fuel
//                           flow equals g/s (airflow boundary for premixed SI)
//            overlay build only: [--real-gas 0|1] [--enthalpy-flow 0|1] [--products 0|1]
//            [--energy 1] (energy balance; see EnergyProbe)

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
#ifdef ENGINE_SIM_OVERLAY
// --energy: per-cylinder energy balance over the measurement window.
// p dV is integrated per simulation step. Each segment between two bottom
// dead centres (volume maxima) is the firing revolution if its peak pressure
// exceeds 3 bar (gross work: compression + expansion), otherwise the gas
// exchange revolution (pumping work). Burn and peak-pressure angles are
// measured from the firing TDC (volume minimum) at the held speed.
struct EnergyProbe {
    struct Cyl {
        double prevV = -1.0, prevP = 0.0, prevDV = 0.0;
        double segWork = 0.0, segPeakP = 0.0, segPeakT = 0.0, tdcT = 0.0;
        std::vector<std::pair<double, double>> burn;   // (time, cumulative burned)
    };
    std::vector<Cyl> cyl;
    double t = 0.0, rpm = 0.0;
    double gross = 0.0, pumping = 0.0;
    long long firings = 0;
    double a10 = 0.0, a50 = 0.0, a90 = 0.0, aPeak = 0.0, peakP = 0.0;
    bool active = false;

    void step(Engine *engine, double dt) {
        t += dt;
        if (cyl.size() != static_cast<size_t>(engine->getCylinderCount())) cyl.resize(engine->getCylinderCount());
        for (int i = 0; i < engine->getCylinderCount(); ++i) {
            CombustionChamber *ch = engine->getChamber(i);
            Cyl &c = cyl[i];
            const double V = ch->getVolume(), p = ch->m_system.pressure();
            if (c.prevV < 0.0) { c.prevV = V; c.prevP = p; continue; }
            const double dV = V - c.prevV;
            c.segWork += 0.5 * (p + c.prevP) * dV;
            if (p > c.segPeakP) { c.segPeakP = p; c.segPeakT = t; }
            if (c.prevDV < 0.0 && dV >= 0.0) c.tdcT = t;              // TDC
            c.burn.emplace_back(t, ch->m_nBurntFuel);
            if (c.prevDV > 0.0 && dV <= 0.0) {                        // BDC: close segment
                if (active) {
                    if (c.segPeakP > 3.0e5) {
                        gross += c.segWork;
                        const double b0 = c.burn.front().second, b1 = c.burn.back().second;
                        if (b1 > b0) {
                            const double degPerS = rpm * 6.0;
                            if (std::getenv("ES_BURN_DUMP") != nullptr && firings == 20) {
                                for (auto &s : c.burn) std::printf("burn %.1f %.4f\n",
                                    (s.first - c.tdcT) * degPerS, (s.second - b0) / (b1 - b0));
                            }
                            double f10 = 0, f50 = 0, f90 = 0;
                            bool h10 = false, h50 = false, h90 = false;
                            for (auto &s : c.burn) {
                                const double x = (s.second - b0) / (b1 - b0);
                                const double deg = (s.first - c.tdcT) * degPerS;
                                if (!h10 && x >= 0.1) { f10 = deg; h10 = true; }
                                if (!h50 && x >= 0.5) { f50 = deg; h50 = true; }
                                if (!h90 && x >= 0.9) { f90 = deg; h90 = true; }
                            }
                            a10 += f10; a50 += f50; a90 += f90;
                            aPeak += (c.segPeakT - c.tdcT) * degPerS;
                            peakP += c.segPeakP;
                            ++firings;
                        }
                    }
                    else pumping += c.segWork;
                }
                c.segWork = 0.0; c.segPeakP = 0.0; c.burn.clear();
            }
            c.prevDV = dV; c.prevV = V; c.prevP = p;
        }
    }
};
#else
struct EnergyProbe { void step(Engine *, double) {} };
#endif

void advance(Simulator *sim, double seconds, int frequency,
             double *torqueSum = nullptr, double *powerSum = nullptr, long long *samples = nullptr,
             Engine *engine = nullptr, double *exhaustGaugeSum = nullptr, EnergyProbe *probe = nullptr,
             double *intakeGaugeSum = nullptr)
{
    const long long steps = static_cast<long long>(seconds * frequency);
    long long done = 0;
    int16_t sink[4096];
    while (done < steps) {
        sim->startFrame(1.0 / 60.0);
        while (done < steps && sim->simulateStep()) {
            ++done;
            if (probe != nullptr) probe->step(engine, 1.0 / frequency);
#ifdef ENGINE_SIM_OVERLAY
            static const bool trace = std::getenv("ES_RUNNER_TRACE") != nullptr;
            static long long traceStep = 0;
            if (trace && engine != nullptr && (++traceStep % std::max(1, std::atoi(std::getenv("ES_RUNNER_TRACE")))) == 0) {
                CombustionChamber *ch = engine->getChamber(0);
                std::printf("trace mdot_in %.4f mdot_ex %.4f | p_cyl %.1f p_inrun %.1f p_plenum %.1f p_exrun %.1f p_exh %.1f kPa T_exh %.1f\n",
                    ch->m_intakeRunnerMassFlow, ch->m_exhaustRunnerMassFlow,
                    ch->m_system.pressure() / 1000, ch->m_intakeRunnerAndManifold.pressure() / 1000,
                    engine->getIntake(0)->getSystem()->pressure() / 1000,
                    ch->m_exhaustRunnerAndPrimary.pressure() / 1000,
                    engine->getExhaustSystem(0)->getSystem()->pressure() / 1000, engine->getExhaustSystem(0)->getSystem()->temperature());
                const ForcedInductionSystem *fi = engine->getForcedInductionSystem();
                if (fi->groupCount() > 0 && fi->group(0)->enabled()) {
                    const TurboGroup::Telemetry &t = fi->group(0)->telemetry();
                    std::printf("turbo p_scroll %.2f T_scroll %.1f mdot_t %.5f P_t %.1f PR_t %.4f p_post %.2f shaft %.1f PR_c %.4f T_post %.1f mdot_c %.5f\n",
                        t.preTurbinePressure.empty() ? 0.0 : t.preTurbinePressure[0] / 1000,
                        t.preTurbineTemperature.empty() ? 0.0 : t.preTurbineTemperature[0],
                        t.turbineMassFlow, t.turbinePower, t.turbinePressureRatio,
                        t.postTurbinePressure / 1000, t.shaftSpeed, t.compressorPressureRatio, t.postTurbineTemperature, t.compressorMassFlow);
                }
            }
#endif
            if (torqueSum != nullptr) {
                *torqueSum += sim->getFilteredDynoTorque();
                *powerSum += sim->getDynoPower();
                // Mean exhaust back-pressure: gauge pressure of the first
                // exhaust system volume (muffler inlet side).
                if (exhaustGaugeSum != nullptr && engine->getExhaustSystemCount() > 0) {
                    *exhaustGaugeSum += engine->getExhaustSystem(0)->getSystem()->pressure()
                        - units::pressure(1.0, units::atm);
                }
                // Mean intake plenum gauge pressure (depression below ambient).
                if (intakeGaugeSum != nullptr && engine->getIntakeCount() > 0) {
                    *intakeGaugeSum += engine->getIntake(0)->getSystem()->pressure()
                        - units::pressure(1.0, units::atm);
                }
                ++*samples;
            }
        }
        sim->endFrame();
        while (sim->readAudioOutput(4096, sink) > 0) {}
    }
}
}

#ifdef ENGINE_SIM_OVERLAY
// --pipe-test: two air volumes joined by one GasSystem::inertialFlow pipe.
// Prints the oscillation period vs the analytic Helmholtz value and the
// energy balance (internal energy + column kinetic energy).
static int pipeTest(double zeta, double dp0) {
    const double V1 = 2.0e-3, V2 = 0.3e-3, A = 10e-4, L = 0.4, T = 300.0, dt = 1.0e-5;
    GasSystem a, b;
    GasSystem::Mix air; air.p_inert = 0.75; air.p_o2 = 0.25; air.p_fuel = 0.0;
    a.initialize(101325.0 + dp0, V1, T, air);
    b.initialize(101325.0, V2, T, air);
    double mdot = 0.0;
    const double gamma = a.heatCapacityRatio();
    const double c = std::sqrt(gamma * 8.314 * T / (a.mass() / a.n()));
    const double omega = std::sqrt(A * c * c / L * (1.0 / V1 + 1.0 / V2));
    const double U0 = a.kineticEnergy() + b.kineticEnergy();
    double lastP = b.pressure(), tUp = -1.0, period = 0.0;
    int ups = 0;
    double maxKE = 0.0, minTot = 1e30, maxTot = -1e30, net = 0.0;
    for (int i = 0; i < 20000; ++i) {
        net += GasSystem::inertialFlow(&a, &b, mdot, A, L, zeta, dt);
        const double rho = a.mass() / a.volume();
        const double ke = 0.5 * rho * A * L * std::pow(mdot / (rho * A), 2);
        const double tot = a.kineticEnergy() + b.kineticEnergy() + ke - U0;
        maxKE = std::max(maxKE, ke); minTot = std::min(minTot, tot); maxTot = std::max(maxTot, tot);
        const double p = b.pressure() - 101325.0 - dp0 * V1 / (V1 + V2);
        if (lastP < 0.0 && p >= 0.0) {
            const double t = i * dt;
            if (tUp >= 0.0) { period += t - tUp; ++ups; }
            tUp = t;
        }
        lastP = p;
        if (i % 2000 == 0)
            std::printf("t %.3f s  p_a %.2f p_b %.2f kPa  mdot %+.4f kg/s  KE %.3f J  dE(U+KE) %+.3f J\n",
                i * dt, a.pressure() / 1000, b.pressure() / 1000, mdot, ke, tot);
    }
    std::printf("zeta %.2f: period sim %.3f ms vs analytic %.3f ms; KE max %.3f J; U+KE drift %+.3f..%+.3f J; net moles a->b %.5f\n",
        zeta, ups ? 1000.0 * period / ups : 0.0, 1000.0 * 2 * 3.14159265 / omega, maxKE, minTot, maxTot, net);
    return 0;
}
#endif

int main(int argc, char **argv) {
#ifdef ENGINE_SIM_OVERLAY
    if (argc >= 4 && std::string(argv[1]) == "--pipe-test") {
        return pipeTest(std::atof(argv[2]), std::atof(argv[3]));
    }
#endif
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
    double throttle = 1.0, settle = 6.0, measure = 3.0, targetTorque = -1.0, targetMap = -1.0, targetFuel = -1.0, crankControl = 0.0;
    int frequency = 0;
    bool energy = false;
    int inertialAfterStart = -1;   // --inertial-after-start 0|1: set after cranking
    bool dynoStart = false;        // --start-mode dyno|self
    for (int i = 3; i + 1 < argc; i += 2) {
        const std::string a = argv[i];
        if (a == "--throttle") throttle = std::atof(argv[i + 1]);
        else if (a == "--settle") settle = std::atof(argv[i + 1]);
        else if (a == "--measure") measure = std::atof(argv[i + 1]);
        else if (a == "--frequency") frequency = std::atoi(argv[i + 1]);
        else if (a == "--torque") targetTorque = std::atof(argv[i + 1]);
        else if (a == "--map") targetMap = std::atof(argv[i + 1]);
        else if (a == "--fuel") targetFuel = std::atof(argv[i + 1]);
        else if (a == "--crank-control") crankControl = std::atof(argv[i + 1]);
        else if (a == "--start-mode") dynoStart = std::string(argv[i + 1]) == "dyno";
        else if (a == "--energy") energy = std::atoi(argv[i + 1]) != 0;
#ifdef ENGINE_SIM_OVERLAY
        // Overlay-only diagnostic switches (value 0 = upstream behaviour).
        else if (a == "--real-gas") gas_vibration::enabled = std::atoi(argv[i + 1]) != 0;
        else if (a == "--products") gas_vibration::products = std::atoi(argv[i + 1]) != 0;
        else if (a == "--enthalpy-flow") gas_vibration::enthalpyFlow = std::atoi(argv[i + 1]) != 0;
        else if (a == "--unified-heat") combustion_physics::unifiedHeatTransfer = std::atoi(argv[i + 1]) != 0;
        else if (a == "--flame-expansion") combustion_physics::flameExpansion = std::atoi(argv[i + 1]) != 0;
        else if (a == "--unbiased-burn") combustion_physics::unbiasedBurnEfficiency = std::atoi(argv[i + 1]) != 0;
        else if (a == "--inertial-runners") combustion_physics::inertialRunners = std::atoi(argv[i + 1]) != 0;
        else if (a == "--inertial-after-start") inertialAfterStart = std::atoi(argv[i + 1]);
#endif
    }

    std::printf("rpm,speed_control,torque_Nm,power_kW,fuel_g_s,bsfc_g_kWh,burned_g_s,exhaust_gauge_kPa,intake_gauge_kPa\n");
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
        engine->setSpeedControl(crankControl);   // --crank-control (default 0)

        // Crank, then run at the requested speed control with the dyno holding.
        sim->m_starterMotor.m_enabled = true;
        if (dynoStart) {
            // --start-mode dyno: engage the dyno while the starter still turns
            // the crank forward (the dyno holds |speed| in the current
            // direction; a failed self-start could otherwise rock backwards
            // and be held in reverse), then let the dyno motor the engine to
            // the test speed at the requested speed control.
            advance(sim, 0.5, f);
            engine->setSpeedControl(throttle);
            sim->m_dyno.m_rotationSpeed = units::rpm(rpm);
            sim->m_dyno.m_maxTorque = units::torque(20000.0, units::ft_lb);
            sim->m_dyno.m_hold = true;
            sim->m_dyno.m_enabled = true;
            advance(sim, 0.3, f);
        }
        else {
            advance(sim, 2.0, f);
        }
        sim->m_starterMotor.m_enabled = false;
#ifdef ENGINE_SIM_OVERLAY
        if (inertialAfterStart >= 0) combustion_physics::inertialRunners = inertialAfterStart != 0;
#endif
        engine->setSpeedControl(throttle);
        sim->m_dyno.m_rotationSpeed = units::rpm(rpm);
        sim->m_dyno.m_maxTorque = units::torque(20000.0, units::ft_lb);
        sim->m_dyno.m_hold = true;
        sim->m_dyno.m_enabled = true;
        advance(sim, settle, f);

        // Part load: bisect the speed control (torque rises monotonically
        // with it) at the held speed, then settle at the result.
        double control = throttle;
        if (targetTorque >= 0.0) {
            double lo = 0.0, hi = 1.0;
            for (int it = 0; it < 12; ++it) {
                control = 0.5 * (lo + hi);
                engine->setSpeedControl(control);
                advance(sim, 1.5, f);
                double t = 0.0, pw = 0.0;
                long long k = 0;
                advance(sim, 0.5, f, &t, &pw, &k);
                t /= std::max(1LL, k);
                pw /= std::max(1LL, k);
                // The dyno torque opposes rotation: compare magnitudes, and
                // treat a motored (absorbing) engine as below target.
                const double brake = pw / std::max(1e-9, std::abs(units::rpm(rpm)));
                if (brake < units::torque(targetTorque, units::Nm)) lo = control; else hi = control;
            }
            control = 0.5 * (lo + hi);
            engine->setSpeedControl(control);
            advance(sim, settle, f);
        }
        // Airflow boundary (premixed intake: metered fuel = air / mass AFR):
        // bisect the speed control until the metered fuel flow equals g/s.
        else if (targetFuel > 0.0) {
            double lo = 0.0, hi = 1.0;
            for (int it = 0; it < 12; ++it) {
                control = 0.5 * (lo + hi);
                engine->setSpeedControl(control);
                advance(sim, 1.0, f);
                const double m0 = engine->getTotalFuelMassConsumed();
                advance(sim, 0.5, f);
                const double gs = (engine->getTotalFuelMassConsumed() - m0) / 0.5 * 1000.0;
                if (gs < targetFuel) lo = control; else hi = control;
            }
            control = 0.5 * (lo + hi);
            engine->setSpeedControl(control);
            advance(sim, settle, f);
        }
        // Manifold-pressure boundary: bisect the speed control until the mean
        // intake plenum pressure equals the measured MAP (absolute kPa).
        else if (targetMap > 0.0) {
            double lo = 0.0, hi = 1.0;
            for (int it = 0; it < 12; ++it) {
                control = 0.5 * (lo + hi);
                engine->setSpeedControl(control);
                advance(sim, 1.0, f);
                double t = 0.0, pw = 0.0, ig = 0.0, eg = 0.0;
                long long k = 0;
                advance(sim, 0.5, f, &t, &pw, &k, engine, &eg, nullptr, &ig);
                const double map = ig / std::max(1LL, k) + units::pressure(1.0, units::atm);
                if (map < units::pressure(targetMap, units::kPa)) lo = control; else hi = control;
            }
            control = 0.5 * (lo + hi);
            engine->setSpeedControl(control);
            advance(sim, settle, f);
        }

#ifdef ENGINE_SIM_OVERLAY
        double burned0 = 0.0, heat0 = 0.0;
        for (int c = 0; c < engine->getCylinderCount(); ++c) {
            burned0 += engine->getChamber(c)->m_nBurntFuel;
            heat0 += engine->getChamber(c)->m_heatLossTotal;
        }
#endif
        EnergyProbe probe;
#ifdef ENGINE_SIM_OVERLAY
        probe.rpm = rpm;
        probe.active = true;
#endif
        const double fuel0 = engine->getTotalFuelMassConsumed();
        double torque = 0.0, power = 0.0;
        long long n = 0;
        double exhaustGauge = 0.0, intakeGauge = 0.0;
        advance(sim, measure, f, &torque, &power, &n, engine, &exhaustGauge, energy ? &probe : nullptr, &intakeGauge);
        const double fuelRate = (engine->getTotalFuelMassConsumed() - fuel0) / measure;   // kg/s
        torque /= std::max(1LL, n);
        power /= std::max(1LL, n);
        const double kW = power / 1000.0;
        double burnedRate = 0.0;
#ifdef ENGINE_SIM_OVERLAY
        for (int c = 0; c < engine->getCylinderCount(); ++c) burnedRate += engine->getChamber(c)->m_nBurntFuel;
        burnedRate = (burnedRate - burned0) / measure;
#endif
        std::printf("%.0f,%.4f,%.2f,%.3f,%.4f,%.1f,%.4f,%.2f,%.2f\n", rpm, control, power / units::rpm(rpm), kW,
            fuelRate * 1000.0, kW > 0.0 ? fuelRate * 1000.0 * 3600.0 / kW : 0.0, burnedRate * 1000.0,
            exhaustGauge / std::max(1LL, n) / 1000.0, intakeGauge / std::max(1LL, n) / 1000.0);
#ifdef ENGINE_SIM_OVERLAY
        if (energy) {
            // Shares of the released heat (burned fuel x LHV) over the window.
            double heat = 0.0;
            for (int c = 0; c < engine->getCylinderCount(); ++c) heat += engine->getChamber(c)->m_heatLossTotal;
            const double released = burnedRate * measure * engine->getFuel()->getEnergyDensity();
            const double brake = power * measure;
            const double net = probe.gross + probe.pumping;
            const double wall = heat - heat0;
            const double k = probe.firings > 0 ? 1.0 / probe.firings : 0.0;
            std::printf("energy: released %.0f J | gross %.3f pumping %.3f net %.3f wall %.3f friction %.3f brake %.3f"
                " residual(exhaust etc.) %.3f | mech eff %.3f | burn 10/50/90 %.1f/%.1f/%.1f deg ATDC,"
                " peak %.1f bar @ %.1f deg (%lld firings)\n",
                released, probe.gross / released, probe.pumping / released, net / released, wall / released,
                (net - brake) / released, brake / released, 1.0 - (net + wall) / released,
                net > 0.0 ? brake / net : 0.0, probe.a10 * k, probe.a50 * k, probe.a90 * k,
                probe.peakP * k / 1.0e5, probe.aPeak * k, probe.firings);
        }
#endif
        std::fflush(stdout);

        sim->destroy();
        compiler.destroy();
    }
    return 0;
}
