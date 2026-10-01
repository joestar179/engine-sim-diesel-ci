#include "../include/intake.h"

#include "../include/units.h"
#include "../include/engine.h"

#include <algorithm>
#include <cmath>

Intake::Intake() {
    m_inputFlowK = 0;
    m_idleFlowK = 0;
    m_flow = 0;
    m_throttle = 1.0;
    m_idleThrottlePlatePosition = 0.0;
    m_crossSectionArea = 0.0;
    m_flowRate = 0;
    m_totalFuelInjected = 0;
    m_molecularAfr = 0;
    m_runnerLength = 0;
    m_airOnly = false;
    m_atmospherePressure = units::pressure(1.0, units::atm);
    m_atmosphereTemperature = units::celcius(25.0);
    m_forcedInductionFeed = false;
    m_engine = nullptr;
}

Intake::~Intake() {
    /* void */
}

void Intake::initialize(Parameters &params) {
    const double width = std::sqrt(params.CrossSectionArea);
    m_system.initialize(
        units::pressure(1.0, units::atm),
        params.volume,
        units::celcius(25.0));
    m_system.setGeometry(
        width,
        params.volume / params.CrossSectionArea,
        1.0,
        0.0);

    m_atmosphere.initialize(
        params.AtmospherePressure,
        units::volume(1000.0, units::m3),
        params.AtmosphereTemperature);
    m_atmosphere.setGeometry(
        units::distance(100.0, units::m),
        units::distance(100.0, units::m),
        1.0,
        0.0);

    m_inputFlowK = params.InputFlowK;
    m_molecularAfr = params.MolecularAfr;
    m_idleFlowK = params.IdleFlowK;
    m_idleThrottlePlatePosition = params.IdleThrottlePlatePosition;
    m_runnerLength = params.RunnerLength;
    m_crossSectionArea = params.CrossSectionArea;
    m_velocityDecay = params.VelocityDecay;
    m_runnerFlowRate = params.RunnerFlowRate;
    m_airOnly = params.AirOnly;
    m_atmospherePressure = params.AtmospherePressure;
    m_atmosphereTemperature = params.AtmosphereTemperature;
}

void Intake::destroy() {
    /* void */
}

void Intake::process(double dt) {
    GasSystem::Mix fuelAirMix;
    GasSystem::Mix fuelMix;
    if (m_airOnly) {
        fuelAirMix.p_fuel = 0.0;
        // Air-only (direct-injection) intakes use real air: 20.95 % O2 by
        // mole. The stock 25 % (kept for premixed spark-ignition intakes and
        // their calibrations) overstates the oxygen a diesel can burn by 19 %.
        fuelAirMix.p_inert = 1.0 - 0.2095;
        fuelAirMix.p_o2 = 0.2095;
        fuelMix = fuelAirMix;
    }
    else {
        const double ideal_afr = 0.8 * m_molecularAfr * 4;
        const double p_air = ideal_afr / (1 + ideal_afr);
        fuelAirMix.p_fuel = 1 - p_air;
        fuelAirMix.p_inert = p_air * 0.75;
        fuelAirMix.p_o2 = p_air * 0.25;

        const double idle_afr = 2.0;
        const double p_idle_air = idle_afr / (1 + idle_afr);
        fuelMix.p_fuel = (1.0 - p_idle_air);
        fuelMix.p_inert = p_idle_air * 0.75;
        fuelMix.p_o2 = p_idle_air * 0.25;
    }

    GasSystem::FlowParameters flowParams;
    flowParams.crossSectionArea_0 = units::area(10, units::m2);
    flowParams.crossSectionArea_1 = m_crossSectionArea;
    flowParams.direction_x = 0.0;
    flowParams.direction_y = -1.0;
    flowParams.dt = dt;
    flowParams.system_0 = &m_atmosphere;
    flowParams.system_1 = &m_system;

    // A configured TurboGroup owns the upstream compressor, cooler, throttle
    // and charge-plenum path. It transfers real gas into m_system later in the
    // same fluid substep. The original atmospheric path is therefore dormant,
    // while the existing intake plenum/runner/valve physics remains unchanged.
    if (m_forcedInductionFeed) {
        m_flow = 0.0;
        m_system.dissipateExcessVelocity();
        m_system.updateVelocity(dt, m_velocityDecay);
        return;
    }

    const double throttle = getThrottlePlatePosition();
    const double flowAttenuation = std::cos(throttle * constants::pi / 2);

    m_atmosphere.reset(m_atmospherePressure, m_atmosphereTemperature, fuelAirMix);
    flowParams.k_flow = flowAttenuation * m_inputFlowK;
    m_flow = GasSystem::flow(flowParams);

    m_atmosphere.reset(m_atmospherePressure, m_atmosphereTemperature, fuelMix);
    flowParams.k_flow = m_idleFlowK;
    const double idleCircuitFlow = GasSystem::flow(flowParams);

    m_system.dissipateExcessVelocity();
    m_system.updateVelocity(dt, m_velocityDecay);

    // Fuel metered = net fuel through the carburettor: reverse flow (intake
    // reversion) returns plenum mixture to the atmosphere and is subtracted,
    // so mixture pumped back and forth is not counted twice. Accounting only.
    const double p_fuelPlenum = m_system.mix().p_fuel;
    m_totalFuelInjected += (m_flow > 0)
        ? fuelAirMix.p_fuel * m_flow
        : p_fuelPlenum * m_flow;
    m_totalFuelInjected += (idleCircuitFlow > 0)
        ? fuelMix.p_fuel * idleCircuitFlow
        : p_fuelPlenum * idleCircuitFlow;
}

void Intake::recordForcedInductionAir(double airMoles) {
    if (airMoles <= 0.0) return;
    m_flow += airMoles;

    // Fuel metering remains an Intake/fuel-system responsibility. The forced-
    // induction component transfers air only and reports the real transferred
    // amount. This preserves existing port-mixture behavior for turbo SI while
    // direct-injected and air-only intakes receive no fuel here.
    if (!m_airOnly) {
        const double idealAfr = 0.8 * m_molecularAfr * 4.0;
        const double fuelMoles = airMoles / std::max(idealAfr, 1.0e-9);
        m_system.injectFuel(fuelMoles);
        m_totalFuelInjected += fuelMoles;
    }
}

