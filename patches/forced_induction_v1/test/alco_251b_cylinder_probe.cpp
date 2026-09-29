// 16-251B per-cylinder combustion and exhaust-blowdown probe.
//
// Diagnostic only; asserts no physics outcome. Starter-driven start with the
// dynamometer disabled and speed control 0 (400-rpm idle target): crank with
// the starter, release, then run unaided.
//
// ProbeSimulator reproduces Engine::createSimulator() exactly and overrides
// PistonEngineSimulator::simulateStep_() with a verbatim copy of the
// production sequence. Only read-only sampling is inserted between the
// production calls, so every fluid sub-step can be observed without changing
// production code. `--check` proves the copy is bit-identical to the stock
// simulator. writeToSynthesizer() (audio only, and the location of the known
// lastValveLift[8] out-of-bounds write for 16 cylinders) is skipped unless
// `--production-audio-path` is given.
//
// usage:
//   engine-sim-cylinder-probe <script.mr> <output-prefix>
//       [--frequency N] [--crank S] [--run S] [--production-audio-path]
//   engine-sim-cylinder-probe <script.mr> --check stock|probe [--frequency N]

#include "../scripting/include/compiler.h"
#include "../include/engine.h"
#include "../include/piston_engine_simulator.h"
#include "../include/units.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr double Rad2Deg = 180.0 / constants::pi;
constexpr double BlowdownWindowDeg = 120.0;
constexpr double TraceWindowSeconds = 0.25;
constexpr int CheckSteps = 8000;

// CombustionChamber keeps its CI event protected. A member pointer formed in
// a derived scope reads it without modifying or instantiating anything.
struct ChamberPeek : CombustionChamber {
    static const CompressionIgnitionModel::Event &event(const CombustionChamber *c) {
        return c->*(&ChamberPeek::m_compressionIgnitionEvent);
    }
};

double wrap360(double deg) {
    // Maps to [-360, 360): signed distance on a 720-degree cycle.
    double x = std::fmod(deg + 360.0, 720.0);
    if (x < 0) x += 720.0;
    return x - 360.0;
}

std::string errorLog() {
    std::ifstream log("error_log.log");
    std::ostringstream details;
    details << log.rdbuf();
    return details.str();
}

struct EventRecord {
    int cylinder = 0;
    int number = 0;
    double time = 0.0;
    double rpm = 0.0;
    bool starter = false;
    double cycleAngleDeg = 0.0;
    double volume = 0.0;
    bool compressing = false;
    bool intakeOpen = false;
    bool exhaustOpen = false;
    double startT = 0.0, startP = 0.0;
    double trappedAirMol = 0.0, o2Mol = 0.0;
    double commandMass = 0.0;
    double burntAtStart = 0.0;
    double injectedMol = 0.0;
    double burnedMass = 0.0;
    bool lit = false;
    double litTime = -1.0, litCycleAngleDeg = 0.0, litT = 0.0, litP = 0.0;
    double peakT = 0.0, peakP = 0.0;
    bool evoSeen = false;
    double evoTime = 0.0, evoCycleAngleDeg = 0.0, evoP = 0.0, evoT = 0.0;
    double evoFuelMol = 0.0, evoO2Mol = 0.0;
    double evoRunnerP = 0.0, evoScrollP = 0.0;
    double runnerPeakP = 0.0, scrollPeakP = 0.0, postPeakP = 0.0;
    double blowdownDeg = 0.0;
    bool blowdownDone = false;
};

struct CylinderState {
    std::string label;
    int bank = 0;
    int indexInBank = 0;
    int scroll = -1;
    double minVolume = std::numeric_limits<double>::infinity();
    double v1 = 0.0, v2 = 0.0; // previous two step volumes
    double a1 = 0.0;            // cycle angle at previous step
    bool i1 = false, e1 = false;
    int openEvent = -1;
    bool exhaustWasOpen = false;
    std::vector<double> firingTdcDeg, overlapTdcDeg;
};

struct SubstepSample {
    double time;
    int starter;
    double rpm, rack;
    double scrollP[4], scrollT[4];
    double postP, postT;
    double turbineMassFlow, turbinePower, turbinePr, shaftRpm;
    double compressorPr, plenumP;
};

class ProbeSimulator;

struct Probe {
    Engine *engine = nullptr;
    TurboGroup *turbo = nullptr;
    int scrollCount = 0;
    double time = 0.0;
    double crankSeconds = 3.0;
    double traceStart = 0.0;
    bool starter = false;
    double fuelMolarMass = 0.0;
    std::vector<CylinderState> cylinders;
    std::vector<EventRecord> events;
    std::ofstream substeps;
    std::ofstream trace;
    long long substepCount = 0;

    double cycleAngleDeg() {
        return engine->getOutputCrankshaft()->getCycleAngle() * Rad2Deg;
    }

    void initialize(Engine *e) {
        engine = e;
        ForcedInductionSystem *fi = engine->getForcedInductionSystem();
        turbo = (fi->enabled() && fi->groupCount() > 0) ? fi->group(0) : nullptr;
        scrollCount = turbo ? static_cast<int>(turbo->telemetry().preTurbinePressure.size()) : 0;
        fuelMolarMass = engine->getFuel()->getMolecularMass();
        const int n = engine->getCylinderCount();
        cylinders.resize(n);
        for (int i = 0; i < n; ++i) {
            Piston *piston = engine->getPiston(i);
            CylinderState &c = cylinders[i];
            c.bank = piston->getCylinderBank()->getIndex();
            c.indexInBank = piston->getCylinderIndex();
            c.label = std::string(c.bank == 0 ? "R" : "L") + std::to_string(c.indexInBank + 1);
            CylinderHead *head = engine->getHead(c.bank);
            GasSystem *destination = engine->getExhaustDestination(head->getExhaustSystem(c.indexInBank));
            for (int k = 0; k < scrollCount; ++k) {
                if (turbo->preTurbineSystem(k) == destination) c.scroll = k;
            }
            const double v = engine->getChamber(i)->getVolume();
            c.v1 = c.v2 = v;
        }
    }

    CylinderHead *head(int i) { return engine->getHead(cylinders[i].bank); }

    // Called where the production loop sees an ignition (injection) event,
    // before beginCompressionIgnitionEvent().
    void onInjectionEvent(int i) {
        CombustionChamber *ch = engine->getChamber(i);
        CylinderState &c = cylinders[i];
        EventRecord r;
        r.cylinder = i;
        r.number = 0;
        for (const EventRecord &e : events) if (e.cylinder == i) ++r.number;
        r.time = time;
        r.rpm = engine->getRpm();
        r.starter = starter;
        r.cycleAngleDeg = cycleAngleDeg();
        r.volume = ch->getVolume();
        r.compressing = r.volume < c.v1;
        r.intakeOpen = head(i)->intakeFlowRate(c.indexInBank) > 0.0;
        r.exhaustOpen = head(i)->exhaustFlowRate(c.indexInBank) > 0.0;
        r.startT = ch->m_system.temperature();
        r.startP = ch->m_system.pressure();
        r.trappedAirMol = ch->getTrappedAirMoles();
        r.o2Mol = ch->m_system.n_o2();
        r.commandMass = engine->getFuelMassPerCycleCommand();
        r.burntAtStart = ch->m_nBurntFuel;
        r.peakT = r.startT;
        r.peakP = r.startP;
        events.push_back(r);
        c.openEvent = static_cast<int>(events.size()) - 1;
        c.exhaustWasOpen = r.exhaustOpen;
    }

    // Called once per simulation step after the production chamber update.
    void afterChamberUpdate() {
        const double angle = cycleAngleDeg();
        for (std::size_t i = 0; i < cylinders.size(); ++i) {
            CylinderState &c = cylinders[i];
            CombustionChamber *ch = engine->getChamber(static_cast<int>(i));
            const double v = ch->getVolume();
            const bool io = head(static_cast<int>(i))->intakeFlowRate(c.indexInBank) > 0.0;
            const bool eo = head(static_cast<int>(i))->exhaustFlowRate(c.indexInBank) > 0.0;
            c.minVolume = std::min(c.minVolume, v);

            // Local volume minimum at the previous step = TDC.
            if (c.v1 < c.v2 && c.v1 <= v) {
                if (!c.i1 && !c.e1) c.firingTdcDeg.push_back(c.a1);
                else c.overlapTdcDeg.push_back(c.a1);
            }
            c.v2 = c.v1; c.v1 = v; c.a1 = angle; c.i1 = io; c.e1 = eo;

            if (c.openEvent >= 0) {
                EventRecord &r = events[c.openEvent];
                if (eo && !c.exhaustWasOpen && !r.evoSeen) {
                    r.evoSeen = true;
                    r.evoTime = time;
                    r.evoCycleAngleDeg = angle;
                    r.evoP = ch->m_system.pressure();
                    r.evoT = ch->m_system.temperature();
                    r.evoFuelMol = ch->m_system.n_fuel();
                    r.evoO2Mol = ch->m_system.n_o2();
                    r.evoRunnerP = ch->m_exhaustRunnerAndPrimary.pressure();
                    r.evoScrollP = c.scroll >= 0 ? turbo->preTurbineSystem(c.scroll)->pressure() : 0.0;
                    r.runnerPeakP = r.evoRunnerP;
                    r.scrollPeakP = r.evoScrollP;
                }
            }
            c.exhaustWasOpen = eo;
        }
    }

    // Called after every production fluid sub-step (after forced induction).
    void afterFluidSubstep(double dt) {
        const double omegaDeg = engine->getSpeed() * Rad2Deg;
        for (std::size_t i = 0; i < cylinders.size(); ++i) {
            CylinderState &c = cylinders[i];
            if (c.openEvent < 0) continue;
            EventRecord &r = events[c.openEvent];
            const CombustionChamber *ch = engine->getChamber(static_cast<int>(i));
            const auto &ev = ChamberPeek::event(ch);
            r.injectedMol = ev.injectedFuelMoles;
            r.burnedMass = ch->m_nBurntFuel - r.burntAtStart;
            if (ev.combustionStarted && !r.lit) {
                r.lit = true;
                r.litTime = time;
                r.litCycleAngleDeg = cycleAngleDeg();
                r.litT = ch->m_system.temperature();
                r.litP = ch->m_system.pressure();
            }
            if (!r.evoSeen) {
                r.peakT = std::max(r.peakT, ch->m_system.temperature());
                r.peakP = std::max(r.peakP, ch->m_system.pressure());
            }
            else if (!r.blowdownDone) {
                r.blowdownDeg += omegaDeg * dt;
                r.runnerPeakP = std::max(r.runnerPeakP, ch->m_exhaustRunnerAndPrimary.pressure());
                if (c.scroll >= 0) {
                    r.scrollPeakP = std::max(r.scrollPeakP, turbo->preTurbineSystem(c.scroll)->pressure());
                }
                if (turbo) r.postPeakP = std::max(r.postPeakP, turbo->telemetry().postTurbinePressure);
                if (r.blowdownDeg >= BlowdownWindowDeg) r.blowdownDone = true;
            }
        }

        ++substepCount;
        if (substeps.is_open() && turbo) {
            const auto &t = turbo->telemetry();
            substeps << time << ',' << (starter ? 1 : 0) << ','
                << engine->getRpm() << ',' << engine->getFuelRack();
            for (int k = 0; k < 4; ++k) substeps << ',' << (k < scrollCount ? t.preTurbinePressure[k] : 0.0);
            for (int k = 0; k < 4; ++k) substeps << ',' << (k < scrollCount ? t.preTurbineTemperature[k] : 0.0);
            substeps << ',' << t.postTurbinePressure << ',' << t.postTurbineTemperature
                << ',' << t.turbineMassFlow << ',' << t.turbinePower << ',' << t.turbinePressureRatio
                << ',' << units::toRpm(t.shaftSpeed) << ',' << t.compressorPressureRatio
                << ',' << t.chargePlenumPressure << '\n';
        }
        if (trace.is_open() && time >= traceStart) {
            trace << time << ',' << cycleAngleDeg();
            for (std::size_t i = 0; i < cylinders.size(); ++i) {
                const CombustionChamber *ch = engine->getChamber(static_cast<int>(i));
                trace << ',' << ch->m_system.pressure() << ',' << ch->m_exhaustRunnerAndPrimary.pressure();
            }
            for (int k = 0; k < scrollCount; ++k) trace << ',' << turbo->preTurbineSystem(k)->pressure();
            if (turbo) trace << ',' << turbo->telemetry().postTurbinePressure;
            trace << '\n';
        }
    }
};

class ProbeSimulator : public PistonEngineSimulator {
public:
    Probe *probe = nullptr;
    bool productionAudioPath = false;

protected:
    // Verbatim copy of PistonEngineSimulator::simulateStep_() with read-only
    // probe calls inserted. Keep in step with src/piston_engine_simulator.cpp;
    // `--check` verifies bit-identical behaviour against the stock simulator.
    void simulateStep_() override {
        const double timestep = getTimestep();
        IgnitionModule *im = m_engine->getIgnitionModule();
        im->update(timestep);

        const int cylinderCount = m_engine->getCylinderCount();
        for (int i = 0; i < cylinderCount; ++i) {
            m_engine->getChamber(i)->resetCombustionPressureRiseRate();
            if (im->getIgnitionEvent(i)) {
                if (probe) probe->onInjectionEvent(i);
                if (m_engine->isCompressionIgnition()) {
                    m_engine->getChamber(i)->beginCompressionIgnitionEvent(
                        m_engine->getFuelMassPerCycleCommand());
                }
                else {
                    m_engine->getChamber(i)->ignite();
                }
            }

            m_engine->getChamber(i)->update(timestep);
        }
        if (probe) probe->afterChamberUpdate();

        for (int i = 0; i < cylinderCount; ++i) {
            m_engine->getChamber(i)->resetLastTimestepExhaustFlow();
            m_engine->getChamber(i)->resetLastTimestepIntakeFlow();
        }

        const int exhaustSystemCount = m_engine->getExhaustSystemCount();
        const int intakeCount = m_engine->getIntakeCount();
        const double fluidTimestep = timestep / m_fluidSimulationSteps;
        for (int i = 0; i < m_fluidSimulationSteps; ++i) {
            for (int j = 0; j < exhaustSystemCount; ++j) {
                m_engine->getExhaustSystem(j)->process(fluidTimestep);
            }

            for (int j = 0; j < intakeCount; ++j) {
                m_engine->getIntake(j)->process(fluidTimestep);
            }

            for (int j = 0; j < cylinderCount; ++j) {
                m_engine->getChamber(j)->flow(fluidTimestep);
            }

            m_engine->processForcedInduction(fluidTimestep);

            for (int j = 0; j < intakeCount; ++j) {
                m_engine->getIntake(j)->m_flowRate += m_engine->getIntake(j)->m_flow;
            }

            if (probe) {
                probe->time += fluidTimestep;
                probe->afterFluidSubstep(fluidTimestep);
            }
        }

        im->resetIgnitionEvents();
    }

    void writeToSynthesizer() override {
        if (productionAudioPath) PistonEngineSimulator::writeToSynthesizer();
    }
};

// Same construction sequence as Engine::createSimulator().
ProbeSimulator *createProbeSimulator(Engine *engine, Vehicle *vehicle, Transmission *transmission) {
    ProbeSimulator *simulator = new ProbeSimulator;
    Simulator::Parameters simulatorParams;
    simulatorParams.systemType = Simulator::SystemType::NsvOptimized;
    simulator->initialize(simulatorParams);
    simulator->loadSimulation(engine, vehicle, transmission);
    simulator->setFluidSimulationSteps(8);
    return simulator;
}

struct Loaded {
    es_script::Compiler compiler;
    Engine *engine = nullptr;
    Vehicle *vehicle = nullptr;
    Transmission *transmission = nullptr;
};

bool load(Loaded &l, const char *script) {
    l.compiler.initialize();
    if (!l.compiler.compile(script)) {
        std::cerr << "probe: script did not compile\n" << errorLog();
        return false;
    }
    auto output = l.compiler.execute();
    l.engine = output.engine;
    l.vehicle = output.vehicle;
    l.transmission = output.transmission;
    if (l.engine == nullptr || l.vehicle == nullptr || l.transmission == nullptr) {
        std::cerr << "probe: script produced no engine/vehicle/transmission\n" << errorLog();
        return false;
    }
    return true;
}

void prepare(Engine *engine, Simulator *sim, int frequency) {
    sim->setSimulationFrequency(frequency);
    sim->m_dyno.m_enabled = false;
    sim->m_dyno.m_hold = false;
    engine->setSpeedControl(0.0);
    engine->getIgnitionModule()->m_enabled = true;
}

// Run `steps` simulation steps with the starter on; frames only schedule steps.
template <typename Step>
void runSteps(Simulator *sim, long long steps, Step &&beforeStep) {
    long long done = 0;
    while (done < steps) {
        sim->startFrame(1.0 / 60.0);
        while (done < steps && sim->simulateStep()) {
            ++done;
            beforeStep(done);
        }
    }
}

int check(const char *script, const std::string &which, int frequency) {
    Loaded l;
    if (!load(l, script)) return 10;
    Simulator *sim = nullptr;
    if (which == "stock") {
        sim = l.engine->createSimulator(l.vehicle, l.transmission);
    }
    else {
        ProbeSimulator *p = createProbeSimulator(l.engine, l.vehicle, l.transmission);
        p->productionAudioPath = true;
        sim = p;
    }
    prepare(l.engine, sim, frequency);
    sim->m_starterMotor.m_enabled = true;
    runSteps(sim, CheckSteps, [](long long) {});

    double sumP = 0.0;
    for (int i = 0; i < l.engine->getCylinderCount(); ++i) sumP += l.engine->getChamber(i)->m_system.pressure();
    std::printf("check %s steps=%d v_theta=%a theta=%a sum_cyl_p=%a burned=%a turbo=%a\n",
        which.c_str(), CheckSteps,
        l.engine->getOutputCrankshaft()->m_body.v_theta,
        l.engine->getOutputCrankshaft()->m_body.theta,
        sumP, l.engine->getDirectBurnedFuelMass(), l.engine->getTurboSpeed());
    l.compiler.destroy();
    return 0;
}

double circularMeanDeg(const std::vector<double> &a, std::size_t from) {
    double s = 0.0, c = 0.0;
    for (std::size_t i = from; i < a.size(); ++i) {
        const double r = a[i] / 720.0 * 2.0 * constants::pi;
        s += std::sin(r);
        c += std::cos(r);
    }
    double m = std::atan2(s, c) / (2.0 * constants::pi) * 720.0;
    return m < 0 ? m + 720.0 : m;
}

} // namespace

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << "usage: engine-sim-cylinder-probe <script.mr> <output-prefix> [--frequency N]"
                     " [--crank S] [--run S] [--production-audio-path]\n"
                     "       engine-sim-cylinder-probe <script.mr> --check stock|probe [--frequency N]\n";
        return 2;
    }
    const char *script = argv[1];
    std::string prefix = argv[2];
    int frequency = 8000;
    double crankSeconds = 3.0;
    double runSeconds = 10.0;
    bool productionAudio = false;
    std::string checkMode;
    for (int i = 2; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--frequency" && i + 1 < argc) frequency = std::atoi(argv[++i]);
        else if (a == "--crank" && i + 1 < argc) crankSeconds = std::atof(argv[++i]);
        else if (a == "--run" && i + 1 < argc) runSeconds = std::atof(argv[++i]);
        else if (a == "--production-audio-path") productionAudio = true;
        else if (a == "--check" && i + 1 < argc) checkMode = argv[++i];
    }
    if (!checkMode.empty()) return check(script, checkMode, frequency);

    Loaded l;
    if (!load(l, script)) return 10;
    Engine *engine = l.engine;
    if (!engine->isCompressionIgnition()) {
        std::cerr << "probe: engine is not compression ignition\n";
        return 11;
    }

    Probe probe;
    probe.crankSeconds = crankSeconds;
    probe.traceStart = crankSeconds + runSeconds - TraceWindowSeconds;
    ProbeSimulator *sim = createProbeSimulator(engine, l.vehicle, l.transmission);
    sim->probe = &probe;
    sim->productionAudioPath = productionAudio;
    prepare(engine, sim, frequency);
    probe.initialize(engine);

    probe.substeps.open(prefix + "_substeps.csv");
    probe.trace.open(prefix + "_blowdown_trace.csv");
    probe.substeps << std::setprecision(7);
    probe.trace << std::setprecision(7);
    probe.substeps << "time_s,starter,rpm,rack,scroll0_p_pa,scroll1_p_pa,scroll2_p_pa,scroll3_p_pa,"
        "scroll0_t_k,scroll1_t_k,scroll2_t_k,scroll3_t_k,post_turbine_p_pa,post_turbine_t_k,"
        "turbine_mass_flow_kg_s,turbine_power_w,turbine_pr,shaft_rpm,compressor_pr,charge_plenum_p_pa\n";
    probe.trace << "time_s,cycle_angle_deg";
    for (const CylinderState &c : probe.cylinders) probe.trace << ',' << c.label << "_cyl_p_pa," << c.label << "_runner_p_pa";
    for (int k = 0; k < probe.scrollCount; ++k) probe.trace << ",scroll" << k << "_p_pa";
    probe.trace << ",post_turbine_p_pa\n";

    const double timestep = 1.0 / frequency;
    const long long crankSteps = static_cast<long long>(std::llround(crankSeconds * frequency));
    const long long totalSteps = static_cast<long long>(std::llround((crankSeconds + runSeconds) * frequency));

    double releaseRpm = 0.0, maxRpm = 0.0, minRpmAfterRelease = std::numeric_limits<double>::infinity();
    double maxShaftRpm = 0.0, maxTurbinePower = 0.0, maxTurbinePr = 1.0, maxScrollP = 0.0;
    double maxPostP = 0.0;
    double rpmSumLast = 0.0; long long rpmNLast = 0;
    probe.starter = true;
    sim->m_starterMotor.m_enabled = true;
    long long done = 0;
    while (done < totalSteps) {
        sim->startFrame(1.0 / 60.0);
        while (done < totalSteps) {
            if (done == crankSteps) {
                releaseRpm = engine->getRpm();
                probe.starter = false;
                sim->m_starterMotor.m_enabled = false;
            }
            if (!sim->simulateStep()) break;
            ++done;
            const double rpm = engine->getRpm();
            maxRpm = std::max(maxRpm, rpm);
            if (done > crankSteps) minRpmAfterRelease = std::min(minRpmAfterRelease, rpm);
            if (done > totalSteps - frequency) { rpmSumLast += rpm; ++rpmNLast; }
            if (probe.turbo) {
                const auto &t = probe.turbo->telemetry();
                maxShaftRpm = std::max(maxShaftRpm, units::toRpm(t.shaftSpeed));
                maxTurbinePower = std::max(maxTurbinePower, t.turbinePower);
                maxTurbinePr = std::max(maxTurbinePr, t.turbinePressureRatio);
                maxPostP = std::max(maxPostP, t.postTurbinePressure);
                for (double p : t.preTurbinePressure) maxScrollP = std::max(maxScrollP, p);
            }
        }
    }
    (void)timestep;
    probe.substeps.close();
    probe.trace.close();

    // Firing TDC per cylinder from the unaided-running half of the detections.
    std::vector<double> firingTdc(probe.cylinders.size(), std::numeric_limits<double>::quiet_NaN());
    for (std::size_t i = 0; i < probe.cylinders.size(); ++i) {
        const auto &f = probe.cylinders[i].firingTdcDeg;
        if (!f.empty()) firingTdc[i] = circularMeanDeg(f, f.size() / 2);
    }

    // Events CSV.
    std::ofstream ev(prefix + "_events.csv");
    ev << std::setprecision(7);
    ev << "cylinder,label,bank,scroll,event,time_s,starter,rpm,cycle_angle_deg,deg_from_firing_tdc,"
          "v_over_vclear,compressing,intake_open,exhaust_open,start_t_k,start_p_pa,trapped_air_mol,o2_mol,"
          "command_fuel_g,injected_fuel_g,burned_fuel_g,lit,lit_deg_from_firing_tdc,lit_t_k,lit_p_pa,"
          "peak_t_k,peak_p_pa,evo_deg_from_firing_tdc,evo_p_pa,evo_t_k,evo_fuel_mol,evo_o2_mol,"
          "evo_runner_p_pa,runner_peak_p_pa,evo_scroll_p_pa,scroll_peak_p_pa,post_turbine_peak_p_pa\n";
    for (const EventRecord &r : probe.events) {
        const CylinderState &c = probe.cylinders[r.cylinder];
        const double tdc = firingTdc[r.cylinder];
        ev << r.cylinder << ',' << c.label << ',' << c.bank << ',' << c.scroll << ',' << r.number << ','
           << r.time << ',' << (r.starter ? 1 : 0) << ',' << r.rpm << ',' << r.cycleAngleDeg << ','
           << wrap360(r.cycleAngleDeg - tdc) << ',' << r.volume / c.minVolume << ','
           << (r.compressing ? 1 : 0) << ',' << (r.intakeOpen ? 1 : 0) << ',' << (r.exhaustOpen ? 1 : 0) << ','
           << r.startT << ',' << r.startP << ',' << r.trappedAirMol << ',' << r.o2Mol << ','
           << r.commandMass * 1000.0 << ',' << r.injectedMol * probe.fuelMolarMass * 1000.0 << ','
           << r.burnedMass * 1000.0 << ',' << (r.lit ? 1 : 0) << ','
           << (r.lit ? wrap360(r.litCycleAngleDeg - tdc) : std::numeric_limits<double>::quiet_NaN()) << ','
           << r.litT << ',' << r.litP << ',' << r.peakT << ',' << r.peakP << ','
           << (r.evoSeen ? wrap360(r.evoCycleAngleDeg - tdc) : std::numeric_limits<double>::quiet_NaN()) << ','
           << r.evoP << ',' << r.evoT << ',' << r.evoFuelMol << ',' << r.evoO2Mol << ','
           << r.evoRunnerP << ',' << r.runnerPeakP << ',' << r.evoScrollP << ',' << r.scrollPeakP << ','
           << r.postPeakP << '\n';
    }
    ev.close();

    // Per-cylinder aggregates over the unaided run (after starter release).
    struct Agg {
        int events = 0, lit = 0, evo = 0;
        double injected = 0, burned = 0, injAngle = 0, vRatio = 0, startT = 0, startP = 0;
        double peakT = 0, peakP = 0, evoP = 0, runnerPeak = 0, scrollPeak = 0, postPeak = 0;
        double injectedAll = 0, burnedAll = 0;
    };
    std::vector<Agg> agg(probe.cylinders.size());
    Agg bankAgg[2];
    for (const EventRecord &r : probe.events) {
        Agg &a = agg[r.cylinder];
        const double inj = r.injectedMol * probe.fuelMolarMass;
        a.injectedAll += inj;
        a.burnedAll += r.burnedMass;
        if (r.starter) continue;
        a.events++;
        a.injected += inj;
        a.burned += r.burnedMass;
        a.lit += r.lit ? 1 : 0;
        a.injAngle += wrap360(r.cycleAngleDeg - firingTdc[r.cylinder]);
        a.vRatio += r.volume / probe.cylinders[r.cylinder].minVolume;
        a.startT += r.startT;
        a.startP += r.startP;
        a.peakT += r.peakT;
        a.peakP += r.peakP;
        if (r.evoSeen && r.blowdownDone) {
            a.evo++;
            a.evoP += r.evoP;
            a.runnerPeak += r.runnerPeakP;
            a.scrollPeak += r.scrollPeakP;
            a.postPeak += r.postPeakP;
        }
        Agg &b = bankAgg[probe.cylinders[r.cylinder].bank];
        b.events++; b.injected += inj; b.burned += r.burnedMass; b.lit += r.lit ? 1 : 0;
    }

    std::ofstream cyl(prefix + "_cylinders.csv");
    cyl << std::setprecision(7);
    cyl << "cylinder,label,bank,scroll,firing_tdc_cycle_deg,overlap_tdc_count,firing_tdc_count,"
           "run_events,run_lit_events,run_injected_g,run_burned_g,run_burned_share,"
           "mean_inj_deg_from_firing_tdc,mean_v_over_vclear_at_inj,mean_start_t_k,mean_start_p_pa,"
           "mean_peak_t_k,mean_peak_p_pa,mean_evo_p_pa,mean_runner_peak_p_pa,mean_scroll_peak_p_pa,"
           "mean_post_turbine_peak_p_pa,all_injected_g,all_burned_g\n";
    auto mean = [](double s, int n) { return n > 0 ? s / n : std::numeric_limits<double>::quiet_NaN(); };
    for (std::size_t i = 0; i < probe.cylinders.size(); ++i) {
        const CylinderState &c = probe.cylinders[i];
        const Agg &a = agg[i];
        cyl << i << ',' << c.label << ',' << c.bank << ',' << c.scroll << ',' << firingTdc[i] << ','
            << c.overlapTdcDeg.size() << ',' << c.firingTdcDeg.size() << ','
            << a.events << ',' << a.lit << ',' << a.injected * 1000 << ',' << a.burned * 1000 << ','
            << (a.injected > 0 ? a.burned / a.injected : 0.0) << ','
            << mean(a.injAngle, a.events) << ',' << mean(a.vRatio, a.events) << ','
            << mean(a.startT, a.events) << ',' << mean(a.startP, a.events) << ','
            << mean(a.peakT, a.events) << ',' << mean(a.peakP, a.events) << ','
            << mean(a.evoP, a.evo) << ',' << mean(a.runnerPeak, a.evo) << ','
            << mean(a.scrollPeak, a.evo) << ',' << mean(a.postPeak, a.evo) << ','
            << a.injectedAll * 1000 << ',' << a.burnedAll * 1000 << '\n';
    }
    cyl.close();

    // Summary.
    std::ofstream s(prefix + "_summary.txt");
    s << std::fixed;
    s << "16-251B per-cylinder probe (diagnostic only; no physics assertion)\n"
      << "script=" << script << "\n"
      << "simulation_frequency_hz=" << frequency << " fluid_substeps=" << sim->getFluidSimulationSteps()
      << " substeps_recorded=" << probe.substepCount << "\n"
      << "audio_path=" << (productionAudio ? "production writeToSynthesizer (includes lastValveLift[8] OOB for 16 cyl)"
                                           : "skipped (no-op writeToSynthesizer; physics unchanged)") << "\n"
      << "fixture: dyno disabled, speed_control=0 (400-rpm target), starter "
      << crankSeconds << " s then released, unaided " << runSeconds << " s\n"
      << "starter_speed_rpm=" << std::setprecision(1) << units::toRpm(sim->m_starterMotor.m_rotationSpeed)
      << " starter_max_torque_lbft=" << units::convert(sim->m_starterMotor.m_maxTorque, units::ft_lb) << "\n\n";

    s << std::setprecision(2)
      << "rpm_at_release=" << releaseRpm << " max_rpm=" << maxRpm
      << " min_rpm_after_release=" << minRpmAfterRelease
      << " mean_rpm_last_1s=" << (rpmNLast ? rpmSumLast / rpmNLast : 0.0) << "\n"
      << std::setprecision(3)
      << "total_injected_g=" << engine->getDirectInjectedFuelMass() * 1000
      << " total_burned_g=" << engine->getDirectBurnedFuelMass() * 1000
      << " share=" << engine->getDirectBurnedFuelMass() / std::max(1e-12, engine->getDirectInjectedFuelMass())
      << "\n\n";

    s << "Per bank, unaided run (events after starter release):\n";
    for (int b = 0; b < 2; ++b) {
        const Agg &a = bankAgg[b];
        s << "  bank " << (b == 0 ? "R" : "L") << ": events=" << a.events << " lit=" << a.lit
          << " injected_g=" << a.injected * 1000 << " burned_g=" << a.burned * 1000
          << " share=" << (a.injected > 0 ? a.burned / a.injected : 0.0) << "\n";
    }

    s << "\nPer cylinder, unaided run:\n"
      << "  cyl  scroll  fTDC_deg  events  lit  inj_g    burn_g   share  inj_deg_vs_fTDC  V/Vc   Tstart  Pstart_MPa"
         "  Tpeak   Ppeak_MPa  P_EVO_kPa  runner_pk_kPa  scroll_pk_kPa\n";
    for (std::size_t i = 0; i < probe.cylinders.size(); ++i) {
        const CylinderState &c = probe.cylinders[i];
        const Agg &a = agg[i];
        char line[512];
        std::snprintf(line, sizeof(line),
            "  %-4s %6d  %8.1f  %6d  %3d  %7.3f  %7.3f  %5.2f  %15.1f  %5.2f  %6.0f  %10.3f  %6.0f  %10.3f  %9.1f  %13.1f  %13.1f\n",
            c.label.c_str(), c.scroll, firingTdc[i], a.events, a.lit, a.injected * 1000, a.burned * 1000,
            a.injected > 0 ? a.burned / a.injected : 0.0, mean(a.injAngle, a.events), mean(a.vRatio, a.events),
            mean(a.startT, a.events), mean(a.startP, a.events) / 1e6, mean(a.peakT, a.events),
            mean(a.peakP, a.events) / 1e6, mean(a.evoP, a.evo) / 1e3, mean(a.runnerPeak, a.evo) / 1e3,
            mean(a.scrollPeak, a.evo) / 1e3);
        s << line;
    }

    s << "\nGeometry used by the blowdown path:\n";
    for (int b = 0; b < engine->getCylinderBankCount(); ++b) {
        CylinderHead *h = engine->getHead(b);
        s << "  head " << b << ": exhaust_runner_volume_L=" << h->getExhaustRunnerVolume() * 1000
          << " exhaust_runner_area_cm2=" << h->getExhaustRunnerCrossSectionArea() * 1e4
          << " clearance_volume_L(min observed)=" << probe.cylinders[b == 0 ? 0 : 8].minVolume * 1000 << "\n";
    }
    for (int k = 0; k < probe.scrollCount; ++k) {
        s << "  scroll " << k << ": volume_L=" << probe.turbo->preTurbineSystem(k)->volume() * 1000 << "\n";
    }
    for (int j = 0; j < engine->getExhaustSystemCount(); ++j) {
        ExhaustSystem *e = engine->getExhaustSystem(j);
        s << "  exhaust_system " << j << ": volume_L=" << e->getSystem()->volume() * 1000
          << " collector_area_cm2=" << e->getCollectorCrossSectionArea() * 1e4 << "\n";
    }

    s << std::setprecision(2) << "\nTurbo maxima over the whole run:\n"
      << "  max_scroll_p_kPa=" << maxScrollP / 1e3 << " max_post_turbine_p_kPa=" << maxPostP / 1e3
      << " max_turbine_pr=" << std::setprecision(4) << maxTurbinePr << std::setprecision(1)
      << " max_turbine_power_W=" << maxTurbinePower << " max_shaft_rpm=" << maxShaftRpm << "\n";
    s << "\nFiles: " << prefix << "_events.csv, _cylinders.csv, _substeps.csv, _blowdown_trace.csv (last "
      << TraceWindowSeconds << " s)\n";
    s.close();

    std::ifstream back(prefix + "_summary.txt");
    std::cout << back.rdbuf();
    l.compiler.destroy();
    return 0;
}
