#include "../include/telemetry_log.h"

#include "../include/engine.h"
#include "../include/simulator.h"
#include "../include/transmission.h"
#include "../include/vehicle.h"
#include "../include/units.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <limits>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace {
double kPa(double p) { return p / 1000.0; }

std::string sanitize(std::string s) {
    for (char &c : s) {
        if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_')) c = '_';
    }
    return s.empty() ? std::string("engine") : s;
}

std::string timestamp(const char *format) {
    const std::time_t now = std::time(nullptr);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), format, &local);
    return buffer;
}

std::string cylinderLabel(Engine *engine, int i) {
    Piston *piston = engine->getPiston(i);
    const int bank = piston->getCylinderBank()->getIndex();
    std::ostringstream s;
    s << "b" << bank << "c" << (piston->getCylinderIndex() + 1);
    return s.str();
}
}

TelemetryLog::TelemetryLog() = default;

TelemetryLog::~TelemetryLog() {
    close();
}

std::string TelemetryLog::defaultDirectory() {
#ifdef _WIN32
    char buffer[MAX_PATH] = {};
    const DWORD n = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    if (n > 0 && n < MAX_PATH) {
        const std::filesystem::path exe(buffer);
        return (exe.parent_path().parent_path() / "logs").string();
    }
#endif
    return "logs";
}

bool TelemetryLog::open(Engine *engine, Simulator *simulator, const std::string &directory, double interval) {
    close();
    if (engine == nullptr || simulator == nullptr) return false;

    std::error_code ec;
    std::filesystem::create_directories(directory, ec);
    const std::filesystem::path file = std::filesystem::path(directory)
        / ("telemetry_" + sanitize(engine->getName()) + "_" + timestamp("%Y%m%d_%H%M%S") + ".log");
    m_file.open(file);
    if (!m_file.is_open()) return false;

    m_path = file.string();
    m_engine = engine;
    m_simulator = simulator;
    m_interval = std::max(0.05, interval);
    m_wallStart = std::chrono::steady_clock::now();
    m_simTime = 0.0;
    m_nextSample = 0.0;
    m_steps = m_frames = m_samples = 0;
    m_first = true;

    m_cylinders.assign(static_cast<std::size_t>(engine->getCylinderCount()), CylinderStats{});
    const ForcedInductionSystem *fi = engine->getForcedInductionSystem();
    const std::size_t scrolls = (fi->enabled() && fi->groupCount() > 0)
        ? fi->group(0)->telemetry().preTurbinePressure.size() : 0;
    m_scrollMax.assign(scrolls, 0.0);
    m_scrollSum.assign(scrolls, 0.0);

    m_file << std::setprecision(6);
    writeHeader();
    resetInterval();
    m_controls = readControls();
    return true;
}

void TelemetryLog::close() {
    if (m_file.is_open()) {
        m_file << "END sim_s=" << m_simTime << " steps=" << m_steps << " samples=" << m_samples << "\n";
        m_file.close();
    }
    m_engine = nullptr;
    m_simulator = nullptr;
}

template <typename T>
void TelemetryLog::kv(const char *key, T value) {
    m_file << ' ' << key << '=' << value;
}

void TelemetryLog::writeHeader() {
    Engine *e = m_engine;
    Simulator *s = m_simulator;
    m_file << "# Engine Simulator telemetry log (read-only; key=value, SI unless the key names a unit)\n"
           << "# SAMPLE lines: snapshot every " << m_interval << " s of simulated time;"
              " *_min/_max/_mean keys cover the interval since the previous SAMPLE.\n"
           << "# Per-cylinder keys use b<bank>c<cylinder-in-bank>. EVENT lines mark operator control changes.\n";
    m_file << "HEADER";
    kv("created", timestamp("%Y-%m-%dT%H:%M:%S"));
    kv("engine", sanitize(e->getName()));
    kv("cylinders", e->getCylinderCount());
    kv("banks", e->getCylinderBankCount());
    kv("crankshafts", e->getCrankshaftCount());
    kv("intakes", e->getIntakeCount());
    kv("exhaust_systems", e->getExhaustSystemCount());
    kv("displacement_L", e->getDisplacement() * 1000.0);
    kv("redline_rpm", units::toRpm(e->getRedline()));
    kv("compression_ignition", e->isCompressionIgnition() ? 1 : 0);
    kv("sim_frequency_hz", s->getSimulationFrequency());
    kv("script_sim_frequency_hz", e->getSimulationFrequency());
    kv("starter_speed_rpm", units::toRpm(e->getStarterSpeed()));
    kv("starter_torque_lbft", units::convert(e->getStarterTorque(), units::ft_lb));
    kv("dyno_min_rpm", units::toRpm(e->getDynoMinSpeed()));
    kv("dyno_max_rpm", units::toRpm(e->getDynoMaxSpeed()));
    kv("crank_tdc_deg", e->getOutputCrankshaft()->getTdc() * 180.0 / constants::pi);
    kv("crank_inertia", e->getOutputCrankshaft()->getMomentOfInertia());
    kv("crank_friction_torque", e->getOutputCrankshaft()->getFrictionTorque());
    m_file << "\n";

    for (int b = 0; b < e->getCylinderBankCount(); ++b) {
        CylinderBank *bank = e->getCylinderBank(b);
        CylinderHead *head = e->getHead(b);
        m_file << "HEADER_BANK";
        kv("bank", b);
        kv("angle_deg", bank->getAngle() * 180.0 / constants::pi);
        kv("bore_mm", bank->getBore() * 1000.0);
        kv("deck_height_mm", bank->getDeckHeight() * 1000.0);
        kv("chamber_volume_cc", head->getCombustionChamberVolume() * 1e6);
        kv("exhaust_runner_volume_L", head->getExhaustRunnerVolume() * 1000.0);
        kv("intake_runner_volume_L", head->getIntakeRunnerVolume() * 1000.0);
        m_file << "\n";
    }

    for (int i = 0; i < e->getCylinderCount(); ++i) {
        Piston *piston = e->getPiston(i);
        const int bank = piston->getCylinderBank()->getIndex();
        CylinderHead *head = e->getHead(bank);
        ExhaustSystem *exhaust = head->getExhaustSystem(piston->getCylinderIndex());
        m_file << "HEADER_CYLINDER";
        kv("index", i);
        kv("label", cylinderLabel(e, i));
        kv("exhaust_system", exhaust->getIndex());
        kv("rod_journal_deg", e->getConnectingRod(i)->getCrankshaft()->getRodJournalAngle(
            e->getConnectingRod(i)->getJournal()) * 180.0 / constants::pi);
        m_file << "\n";
    }

    if (e->isCompressionIgnition()) {
        const auto &p = e->getCompressionIgnitionModel()->parameters();
        m_file << "HEADER_CI";
        kv("max_fuel_per_cycle_g", p.maxFuelMassPerCycle * 1000.0);
        kv("injection_duration_deg", p.injectionDuration * 180.0 / constants::pi);
        kv("ignition_delay_deg", p.ignitionDelay * 180.0 / constants::pi);
        kv("combustion_duration_deg", p.combustionDuration * 180.0 / constants::pi);
        kv("premixed_fraction", p.premixedBurnFraction);
        kv("autoignition_T", p.autoignitionTemperature);
        kv("autoignition_P", p.autoignitionPressure);
        m_file << "\n";
    }

    const ForcedInductionSystem *fi = e->getForcedInductionSystem();
    for (std::size_t g = 0; g < fi->groupCount(); ++g) {
        const TurboGroup::Parameters &p = fi->group(g)->parameters();
        m_file << "HEADER_TURBO";
        kv("group", g);
        kv("enabled", p.enabled ? 1 : 0);
        kv("shaft_inertia", p.shaftInertia);
        kv("friction_torque", p.frictionTorque);
        kv("max_speed_rpm", units::toRpm(p.maxSpeed));
        kv("max_pr", p.maxPressureRatio);
        kv("compressor_eff", p.compressorEfficiency);
        kv("turbine_eff", p.turbineEfficiency);
        kv("design_mass_flow", p.designMassFlow);
        kv("turbine_design_pr", p.turbineDesignPressureRatio);
        kv("turbine_design_T", p.turbineDesignTemperature);
        kv("pre_turbine_volume_L", p.preTurbineVolume * 1000.0);
        kv("scrolls", p.inletChannelCount);
        kv("compressor_inlet_volume_L", p.compressorInletVolume * 1000.0);
        kv("compressor_discharge_volume_L", p.compressorDischargeVolume * 1000.0);
        kv("cooler_volume_L", p.coolerVolume * 1000.0);
        kv("charge_plenum_volume_L", p.chargePlenumVolume * 1000.0);
        kv("cooler", p.chargeAirCoolerEnabled ? 1 : 0);
        kv("cooler_effectiveness", p.aftercoolerEffectiveness);
        kv("throttle", p.throttleEnabled ? 1 : 0);
        kv("wastegate", p.wastegateEnabled ? 1 : 0);
        kv("bypass", p.compressorBypassEnabled ? 1 : 0);
        kv("vgt", p.vgtEnabled ? 1 : 0);
        kv("post_turbine_exhaust", p.postTurbineExhaustIndex);
        m_file << "\n";
    }
    m_file.flush();
}

TelemetryLog::Controls TelemetryLog::readControls() const {
    Controls c;
    c.starter = m_simulator->m_starterMotor.m_enabled;
    c.ignition = m_engine->getIgnitionModule()->m_enabled;
    c.dyno = m_simulator->m_dyno.m_enabled;
    c.hold = m_simulator->m_dyno.m_hold;
    c.dynoSpeed = m_simulator->m_dyno.m_rotationSpeed;
    c.gear = m_simulator->getTransmission() ? m_simulator->getTransmission()->getGear() : -2;
    c.speedControl = m_engine->getSpeedControl();
    return c;
}

void TelemetryLog::checkControls() {
    const Controls c = readControls();
    auto event = [&](const char *what, const std::string &value) {
        m_file << "EVENT";
        kv("sim_s", m_simTime);
        kv("rpm", m_engine->getRpm());
        kv(what, value);
        m_file << "\n";
    };
    if (c.starter != m_controls.starter) event("starter", c.starter ? "on" : "off");
    if (c.ignition != m_controls.ignition) event("ignition", c.ignition ? "on" : "off");
    if (c.dyno != m_controls.dyno) event("dyno", c.dyno ? "on" : "off");
    if (c.hold != m_controls.hold) event("dyno_hold", c.hold ? "on" : "off");
    if (c.gear != m_controls.gear) event("gear", std::to_string(c.gear));
    if (std::abs(c.speedControl - m_controls.speedControl) > 0.02) {
        std::ostringstream v;
        v << std::setprecision(3) << c.speedControl;
        event("speed_control", v.str());
    }
    else if (std::abs(c.dynoSpeed - m_controls.dynoSpeed) > units::rpm(25.0)) {
        std::ostringstream v;
        v << std::setprecision(5) << units::toRpm(c.dynoSpeed);
        event("dyno_speed_rpm", v.str());
    }
    else {
        // Keep the last reported values so slow ramps still produce events.
        m_controls = Controls{ c.starter, c.ignition, c.dyno, c.hold,
            m_controls.dynoSpeed, c.gear, m_controls.speedControl };
        return;
    }
    m_controls = c;
}

void TelemetryLog::resetInterval() {
    m_intervalSteps = 0;
    m_intervalStart = m_simTime;
    const double rpm = m_engine ? m_engine->getRpm() : 0.0;
    const double rack = m_engine ? m_engine->getFuelRack() : 0.0;
    m_rpmMin = m_rpmMax = rpm;
    m_rpmSum = 0.0;
    m_rackMin = m_rackMax = rack;
    m_rackSum = 0.0;
    m_turbineInletMax = m_turbinePowerMax = m_turbinePowerSum = 0.0;
    m_turbineMassFlowSum = m_compressorMassFlowSum = m_intakeMassFlowSum = 0.0;
    m_plenumMin = std::numeric_limits<double>::infinity();
    m_plenumMax = m_plenumSum = 0.0;
    m_dynoTorqueSum = 0.0;
    if (m_engine) {
        m_injectedAtStart = m_engine->getDirectInjectedFuelMass();
        m_burnedAtStart = m_engine->getDirectBurnedFuelMass();
    }
    std::fill(m_scrollMax.begin(), m_scrollMax.end(), 0.0);
    std::fill(m_scrollSum.begin(), m_scrollSum.end(), 0.0);
    for (std::size_t i = 0; i < m_cylinders.size(); ++i) {
        CylinderStats &c = m_cylinders[i];
        CombustionChamber *ch = m_engine->getChamber(static_cast<int>(i));
        c.peakPressure = ch->m_system.pressure();
        c.peakTemperature = ch->m_system.temperature();
        c.runnerPeakPressure = ch->m_exhaustRunnerAndPrimary.pressure();
        c.burntAtStart = ch->m_nBurntFuel;
        c.litEdges = 0;
    }
}

void TelemetryLog::sampleStep() {
    if (!m_file.is_open()) return;
    ++m_steps;
    ++m_intervalSteps;
    m_simTime += m_simulator->getTimestep();

    const double rpm = m_engine->getRpm();
    const double rack = m_engine->getFuelRack();
    m_rpmMin = std::min(m_rpmMin, rpm);
    m_rpmMax = std::max(m_rpmMax, rpm);
    m_rpmSum += rpm;
    m_rackMin = std::min(m_rackMin, rack);
    m_rackMax = std::max(m_rackMax, rack);
    m_rackSum += rack;
    m_dynoTorqueSum += m_simulator->m_dyno.getTorque();

    const ForcedInductionSystem *fi = m_engine->getForcedInductionSystem();
    if (fi->enabled() && fi->groupCount() > 0) {
        const auto &t = fi->group(0)->telemetry();
        for (std::size_t k = 0; k < m_scrollMax.size() && k < t.preTurbinePressure.size(); ++k) {
            m_scrollMax[k] = std::max(m_scrollMax[k], t.preTurbinePressure[k]);
            m_scrollSum[k] += t.preTurbinePressure[k];
            m_turbineInletMax = std::max(m_turbineInletMax, t.preTurbinePressure[k]);
        }
        m_turbinePowerMax = std::max(m_turbinePowerMax, t.turbinePower);
        m_turbinePowerSum += t.turbinePower;
        m_turbineMassFlowSum += t.turbineMassFlow;
        m_compressorMassFlowSum += t.compressorMassFlow;
        m_intakeMassFlowSum += t.intakeMassFlow;
        m_plenumMin = std::min(m_plenumMin, t.chargePlenumPressure);
        m_plenumMax = std::max(m_plenumMax, t.chargePlenumPressure);
        m_plenumSum += t.chargePlenumPressure;
    }

    for (std::size_t i = 0; i < m_cylinders.size(); ++i) {
        CylinderStats &c = m_cylinders[i];
        const CombustionChamber *ch = m_engine->getChamber(static_cast<int>(i));
        c.peakPressure = std::max(c.peakPressure, ch->m_system.pressure());
        c.peakTemperature = std::max(c.peakTemperature, ch->m_system.temperature());
        c.runnerPeakPressure = std::max(c.runnerPeakPressure, ch->getExhaustRunnerPeakPressure());
        const bool lit = ch->isLit();
        if (lit && !c.wasLit) ++c.litEdges;
        c.wasLit = lit;
    }
}

void TelemetryLog::endFrame() {
    if (!m_file.is_open()) return;
    ++m_frames;
    if (m_first) {
        m_first = false;
        writeSnapshot();
        return;
    }
    checkControls();
    if (m_simTime >= m_nextSample && m_intervalSteps > 0) writeSnapshot();
}

void TelemetryLog::writeSnapshot() {
    Engine *e = m_engine;
    Simulator *s = m_simulator;
    const double wall = std::chrono::duration<double>(std::chrono::steady_clock::now() - m_wallStart).count();
    const double n = static_cast<double>(std::max<long long>(1, m_intervalSteps));
    const double dtInterval = std::max(1e-9, m_simTime - m_intervalStart);

    m_file << "SAMPLE";
    kv("n", m_samples);
    kv("sim_s", m_simTime);
    kv("wall_s", wall);
    kv("frames", m_frames);
    kv("steps", m_steps);
    kv("interval_steps", m_intervalSteps);
    kv("sim_speed", s->getSimulationSpeed());
    kv("steps_per_frame", s->getFrameIterationCount());
    kv("physics_us_per_frame", s->getAverageProcessingTime());
    kv("synth_latency", s->getSynthesizerInputLatency());

    // Crank and controls.
    Crankshaft *crank = e->getOutputCrankshaft();
    kv("rpm", e->getRpm());
    kv("rpm_min", m_rpmMin);
    kv("rpm_max", m_rpmMax);
    kv("rpm_mean", m_rpmSum / n);
    kv("filtered_rpm", units::toRpm(s->filteredEngineSpeed()));
    kv("omega_signed", crank->m_body.v_theta);
    kv("cycle_angle_deg", crank->getCycleAngle() * 180.0 / constants::pi);
    kv("speed_control", e->getSpeedControl());
    kv("throttle", e->getThrottle());
    kv("rack", e->getFuelRack());
    kv("rack_min", m_rackMin);
    kv("rack_max", m_rackMax);
    kv("rack_mean", m_rackSum / n);
    kv("fuel_cmd_g", e->getFuelMassPerCycleCommand() * 1000.0);
    kv("component_friction_Nm", e->getComponentFrictionTorque());
    kv("ignition", e->getIgnitionModule()->m_enabled ? 1 : 0);
    kv("timing_adv_deg", e->getIgnitionModule()->getTimingAdvance() * 180.0 / constants::pi);
    kv("starter", s->m_starterMotor.m_enabled ? 1 : 0);
    kv("dyno", s->m_dyno.m_enabled ? 1 : 0);
    kv("dyno_hold", s->m_dyno.m_hold ? 1 : 0);
    kv("dyno_set_rpm", units::toRpm(s->m_dyno.m_rotationSpeed));
    kv("dyno_torque_Nm_mean", m_dynoTorqueSum / n);
    kv("dyno_torque_filtered_Nm", s->getFilteredDynoTorque());
    kv("dyno_power_kW", s->getDynoPower() / 1000.0);
    if (Transmission *tr = s->getTransmission()) {
        kv("gear", tr->getGear());
        kv("clutch", tr->getClutchPressure());
    }
    if (Vehicle *v = s->getVehicle()) kv("vehicle_kmh", v->getSpeed() * 3.6);

    // Fuel and combustion.
    const double injected = e->getDirectInjectedFuelMass() - m_injectedAtStart;
    const double burned = e->getDirectBurnedFuelMass() - m_burnedAtStart;
    kv("inj_total_g", e->getDirectInjectedFuelMass() * 1000.0);
    kv("burn_total_g", e->getDirectBurnedFuelMass() * 1000.0);
    kv("inj_interval_g", injected * 1000.0);
    kv("burn_interval_g", burned * 1000.0);
    kv("burn_share_interval", injected > 0.0 ? burned / injected : 0.0);
    kv("fuel_rate_g_s", injected * 1000.0 / dtInterval);
    kv("fuel_consumed_total_g", e->getTotalFuelMassConsumed() * 1000.0);
    kv("ci_max_T", e->getMaxCompressionIgnitionTemperature());
    kv("ci_max_P_MPa", e->getMaxCompressionIgnitionPressure() / 1e6);
    kv("manifold_kPa", kPa(e->getManifoldPressure()));
    kv("intake_afr", e->getIntakeAfr());
    kv("exhaust_o2", e->getExhaustO2());
    kv("intake_flow", e->getIntakeFlowRate());

    // Intakes and exhaust systems.
    for (int i = 0; i < e->getIntakeCount(); ++i) {
        Intake *in = e->getIntake(i);
        const std::string p = "in" + std::to_string(i) + "_";
        kv(p + "kPa", kPa(in->getSystem()->pressure()));
        kv(p + "T", in->getSystem()->temperature());
        kv(p + "o2_frac", in->getSystem()->n() > 0 ? in->getSystem()->n_o2() / in->getSystem()->n() : 0.0);
        kv(p + "throttle_plate", in->getThrottlePlatePosition());
        kv(p + "flow", in->m_flowRate);
    }
    for (int i = 0; i < e->getExhaustSystemCount(); ++i) {
        ExhaustSystem *ex = e->getExhaustSystem(i);
        const std::string p = "ex" + std::to_string(i) + "_";
        kv(p + "kPa", kPa(ex->getSystem()->pressure()));
        kv(p + "T", ex->getSystem()->temperature());
        kv(p + "flow", ex->getFlow());
    }

    // Turbo groups.
    const ForcedInductionSystem *fi = e->getForcedInductionSystem();
    kv("turbo_groups", fi->enabled() ? static_cast<int>(fi->groupCount()) : 0);
    for (std::size_t g = 0; fi->enabled() && g < fi->groupCount(); ++g) {
        const auto &t = fi->group(g)->telemetry();
        const std::string p = "t" + std::to_string(g) + "_";
        kv(p + "shaft_rpm", units::toRpm(t.shaftSpeed));
        for (std::size_t k = 0; k < t.preTurbinePressure.size(); ++k) {
            const std::string sk = p + "scroll" + std::to_string(k) + "_";
            kv(sk + "kPa", kPa(t.preTurbinePressure[k]));
            kv(sk + "T", t.preTurbineTemperature[k]);
            if (g == 0 && k < m_scrollMax.size()) {
                kv(sk + "kPa_max", kPa(m_scrollMax[k]));
                kv(sk + "kPa_mean", kPa(m_scrollSum[k] / n));
            }
        }
        kv(p + "post_kPa", kPa(t.postTurbinePressure));
        kv(p + "post_T", t.postTurbineTemperature);
        kv(p + "turbine_pr", t.turbinePressureRatio);
        kv(p + "turbine_kW", t.turbinePower / 1000.0);
        kv(p + "turbine_kg_s", t.turbineMassFlow);
        kv(p + "wastegate_kg_s", t.wastegateMassFlow);
        kv(p + "comp_pr", t.compressorPressureRatio);
        kv(p + "comp_kW", t.compressorPower / 1000.0);
        kv(p + "comp_kg_s", t.compressorMassFlow);
        kv(p + "comp_out_T", t.compressorDischargeTemperature);
        kv(p + "cooler_in_kPa", kPa(t.coolerInletPressure));
        kv(p + "cooler_in_T", t.coolerInletTemperature);
        kv(p + "cooler_out_kPa", kPa(t.coolerOutletPressure));
        kv(p + "cooler_out_T", t.coolerOutletTemperature);
        kv(p + "plenum_kPa", kPa(t.chargePlenumPressure));
        kv(p + "plenum_T", t.chargePlenumTemperature);
        kv(p + "intake_kg_s", t.intakeMassFlow);
        kv(p + "bypass_kg_s", t.bypassMassFlow);
        kv(p + "wastegate_pos", t.wastegatePosition);
        kv(p + "bypass_pos", t.compressorBypassPosition);
        kv(p + "vgt_pos", t.vgtPosition);
        if (g == 0) {
            kv(p + "scroll_kPa_max", kPa(m_turbineInletMax));
            kv(p + "turbine_kW_max", m_turbinePowerMax / 1000.0);
            kv(p + "turbine_kW_mean", m_turbinePowerSum / n / 1000.0);
            kv(p + "turbine_kg_s_mean", m_turbineMassFlowSum / n);
            kv(p + "comp_kg_s_mean", m_compressorMassFlowSum / n);
            kv(p + "intake_kg_s_mean", m_intakeMassFlowSum / n);
            kv(p + "plenum_kPa_min", kPa(std::isfinite(m_plenumMin) ? m_plenumMin : 0.0));
            kv(p + "plenum_kPa_max", kPa(m_plenumMax));
            kv(p + "plenum_kPa_mean", kPa(m_plenumSum / n));
        }
    }

    // Cylinders.
    for (int i = 0; i < e->getCylinderCount(); ++i) {
        CombustionChamber *ch = e->getChamber(i);
        const CylinderStats &c = m_cylinders[static_cast<std::size_t>(i)];
        const std::string p = cylinderLabel(e, i) + "_";
        const GasSystem &g = ch->m_system;
        kv(p + "kPa", kPa(g.pressure()));
        kv(p + "T", g.temperature());
        kv(p + "V_L", ch->getVolume() * 1000.0);
        kv(p + "air_mol", ch->getTrappedAirMoles());
        kv(p + "o2_mol", g.n_o2());
        kv(p + "fuel_mol", g.n_fuel());
        kv(p + "lit", ch->isLit() ? 1 : 0);
        kv(p + "lit_count", c.litEdges);
        kv(p + "peak_kPa", kPa(c.peakPressure));
        kv(p + "peak_T", c.peakTemperature);
        kv(p + "burn_g", (ch->m_nBurntFuel - c.burntAtStart) * 1000.0);
        kv(p + "evo_kPa", kPa(ch->getExhaustValveOpeningPressure()));
        kv(p + "evo_T", ch->getExhaustValveOpeningTemperature());
        kv(p + "runner_kPa", kPa(ch->m_exhaustRunnerAndPrimary.pressure()));
        kv(p + "runner_peak_kPa", kPa(c.runnerPeakPressure));
        kv(p + "intake_runner_kPa", kPa(ch->m_intakeRunnerAndManifold.pressure()));
        kv(p + "max_T_ever", ch->m_peakTemperature);
    }
    m_file << "\n";
    m_file.flush();

    ++m_samples;
    while (m_nextSample <= m_simTime) m_nextSample += m_interval;
    resetInterval();
}
