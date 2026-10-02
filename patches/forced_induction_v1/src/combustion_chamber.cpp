#include "../include/combustion_chamber.h"

#include "../include/constants.h"
#include "../include/units.h"
#include "../include/piston.h"
#include "../include/connecting_rod.h"
#include "../include/utilities.h"
#include "../include/exhaust_system.h"
#include "../include/cylinder_bank.h"
#include "../include/engine.h"

#include <algorithm>
#include <cmath>

CombustionChamber::CombustionChamber() {
    m_crankcasePressure = 0.0;
    m_piston = nullptr;
    m_head = nullptr;
    m_engine = nullptr;
    m_pistonSpeed = nullptr;
    m_pressure = nullptr;
    m_lit = false;
    m_litLastFrame = false;
    m_peakTemperature = 0;

    m_meanPistonSpeedToTurbulence = nullptr;
    m_nBurntFuel = 0;
    m_heatLossTotal = 0;

    m_manifoldToRunnerFlowRate = 0;
    m_primaryToCollectorFlowRate = 0;
    m_cylinderWidthApproximation = 0;
    m_cylinderCrossSectionSurfaceArea = 0;

    m_lastTimestepTotalExhaustFlow = 0;
    m_lastTimestepTotalIntakeFlow = 0;
    m_exhaustFlow = 0;
    m_exhaustFlowRate = 0;
    m_intakeFlowRate = 0;

    m_fuel = nullptr;
    m_combustionPressureRiseRate = 0.0;
    m_exhaustValveOpeningPressure = 0.0;
    m_exhaustValveOpeningTemperature = 0.0;
    m_exhaustRunnerPeakPressure = 0.0;
    m_exhaustRunnerPeakTemperature = 0.0;
    m_exhaustValveWasOpen = false;
}

CombustionChamber::~CombustionChamber() {
    assert(m_pistonSpeed == nullptr);
    assert(m_pressure == nullptr);
}

void CombustionChamber::initialize(const Parameters &params) {
    m_piston = params.Piston;
    m_head = params.Head;
    m_fuel = params.Fuel;
    m_crankcasePressure = params.CrankcasePressure;
    m_meanPistonSpeedToTurbulence = params.MeanPistonSpeedToTurbulence;
    m_chamberAreaRatio = params.ChamberAreaRatio;
    m_pistonWallTemperature = params.PistonWallTemperature;
    m_headWallTemperature = params.HeadWallTemperature;
    m_linerWallTemperature = params.LinerWallTemperature;

    m_pistonSpeed = new double[StateSamples];
    m_pressure = new double[StateSamples];
    for (int i = 0; i < StateSamples; ++i) {
        m_pistonSpeed[i] = 0;
        m_pressure[i] = 0;
    }

    Intake *intake = m_head->getIntake(m_piston->getCylinderIndex());
    ExhaustSystem *exhaust = m_head->getExhaustSystem(m_piston->getCylinderIndex());

    m_manifoldToRunnerFlowRate = intake->getRunnerFlowRate();
    m_primaryToCollectorFlowRate = exhaust->getPrimaryFlowRate();

    const double bore_r = m_head->getCylinderBank()->getBore() / 2.0;
    m_cylinderCrossSectionSurfaceArea = constants::pi * bore_r * bore_r;
    m_cylinderWidthApproximation = std::sqrt(m_cylinderCrossSectionSurfaceArea);

    const double height = getVolume() / m_cylinderCrossSectionSurfaceArea;
    m_system.setGeometry(
        m_cylinderWidthApproximation,
        height,
        1.0,
        0.0);

    const double intakeRunnerCrossSection = m_head->getIntakeRunnerCrossSectionArea();
    const double intakeRunnerWidth = std::sqrt(intakeRunnerCrossSection);
    const double manifoldRunnerLength = intake->getRunnerLength();
    const double manifoldRunnerVolume = intakeRunnerCrossSection * manifoldRunnerLength;
    const double totalIntakeRunnerVolume = m_head->getIntakeRunnerVolume() + manifoldRunnerVolume;
    const double overallIntakeRunnerLength = totalIntakeRunnerVolume / intakeRunnerCrossSection;
    m_intakeRunnerLength = overallIntakeRunnerLength;
    // Inertial runners: the runner pipe (manifold runner + port) is the
    // Helmholtz neck; only the port volume remains as the lumped volume at
    // the valve (the cylinder is the cavity). Keeping the whole runner volume
    // as well double-counted the gas as both inertia and compliance and
    // created a spurious plenum-runner resonance.
    m_intakeRunnerAndManifold.initialize(
        units::pressure(1.0, units::atm),
        combustion_physics::inertialRunners
            ? m_head->getIntakeRunnerVolume() + combustion_physics::pipeCavityShare * manifoldRunnerVolume
            : totalIntakeRunnerVolume,
        units::celcius(25.0));
    m_intakeRunnerAndManifold.setGeometry(
        overallIntakeRunnerLength,
        intakeRunnerWidth,
        1.0,
        0.0);

    const double exhaustRunnerCrossSection = m_head->getExhaustRunnerCrossSectionArea();
    const double exhaustRunnerWidth = std::sqrt(exhaustRunnerCrossSection);
    const double exhaustTubeLength =
        exhaust->getPrimaryTubeLength() + m_head->getHeaderPrimaryLength(m_piston->getCylinderIndex());
    const double exhaustTubeVolume = exhaustRunnerCrossSection * exhaustTubeLength;
    const double totalExhaustRunnerVolume = m_head->getExhaustRunnerVolume() + exhaustTubeVolume;
    const double overallExhaustRunnerLength = totalExhaustRunnerVolume / exhaustRunnerCrossSection;
    m_exhaustRunnerLength = overallExhaustRunnerLength;
    m_exhaustRunnerAndPrimary.initialize(
        units::pressure(1.0, units::atm),
        (combustion_physics::inertialRunners && combustion_physics::inertialExhaust)
            ? m_head->getExhaustRunnerVolume() + combustion_physics::pipeCavityShare * exhaustTubeVolume
            : totalExhaustRunnerVolume,
        units::celcius(25.0));
    m_exhaustRunnerAndPrimary.setGeometry(
        overallExhaustRunnerLength,
        exhaustRunnerWidth,
        1.0,
        0.0);
}

void CombustionChamber::destroy() {
    if (m_pistonSpeed != nullptr) delete[] m_pistonSpeed;
    if (m_pressure != nullptr) delete[] m_pressure;

    m_pistonSpeed = nullptr;
    m_pressure = nullptr;
}

double CombustionChamber::getVolume() const {
    const double combustionPortVolume = m_head->getCombustionChamberVolume();
    const CylinderBank *bank = m_head->getCylinderBank();

    const double area = bank->boreSurfaceArea();
    const double s =
        m_piston->relativeX() * bank->getDx()
        + m_piston->relativeY() * bank->getDy();
    const double sweep =
        area * (bank->getDeckHeight() - s - m_piston->getCompressionHeight());

    return sweep + combustionPortVolume - m_piston->getDisplacement();
}

double CombustionChamber::pistonSpeed() const {
    const CylinderBank *bank = m_head->getCylinderBank();
    return
        m_piston->m_body.v_x * bank->getDx()
        + m_piston->m_body.v_y * bank->getDy();
}

double CombustionChamber::calculateMeanPistonSpeed() const {
    double avg = 0;
    for (int i = 0; i < StateSamples; ++i) {
        avg += m_pistonSpeed[i];
    }

    avg /= StateSamples;
    return avg;
}

double CombustionChamber::calculateFiringPressure() const {
    double firingPressure = 0;
    for (int i = 0; i < StateSamples; ++i) {
        if (m_pressure[i] > firingPressure) {
            firingPressure = m_pressure[i];
        }
    }

    return firingPressure;
}

bool CombustionChamber::popLitLastFrame() {
    const bool lit = m_litLastFrame;
    m_litLastFrame = false;

    return lit;
}

void CombustionChamber::ignite() {
    if (!m_lit) {
        if (m_system.mix().p_fuel == 0) return;

        const double afr = m_system.mix().p_o2 / m_system.mix().p_fuel;
        const double equivalenceRatio = afr / m_fuel->getMolecularAfr();
        if (equivalenceRatio < 0.5) return;
        else if (equivalenceRatio > 1.9) return;

        const double idealInert = m_system.mix().p_o2 / 0.7;
        const double dilution = (m_system.mix().p_inert / idealInert) - 1;

        m_flameEvent.lastVolume = getVolume();
        m_flameEvent.travel_x = 0;
        m_flameEvent.travel_y = 0;
        m_flameEvent.lit_n = 0;
        m_flameEvent.total_n = m_system.n();
        m_flameEvent.percentageLit = 0;
        m_flameEvent.globalMix = m_system.mix();
        m_lit = true;
        m_litLastFrame = true;

        const double randomness =
            m_fuel->getBurningEfficiencyRandomness();
        const double lowEfficiencyAttenuation =
            m_fuel->getLowEfficiencyAttenuation();
        const double maxBurningEfficiency =
            m_fuel->getMaxBurningEfficiency();
        const double maxTurbulenceEffect =
            m_fuel->getMaxTurbulenceEffect();
        const double maxDilutionEffect =
            m_fuel->getMaxDilutionEffect();

        const double turbulence =
            m_meanPistonSpeedToTurbulence->sampleTriangle(
                calculateMeanPistonSpeed());
        const double mixingFactor =
            1.0 - (
                clamp(turbulence / maxTurbulenceEffect)
                * clamp(1 - dilution / maxDilutionEffect));
        const double u = (double)rand() / RAND_MAX;
        double efficiencyAttenuation;
        if (combustion_physics::unbiasedBurnEfficiency) {
            // Combustion efficiency is the fuel's (sourced, lean/stoichiometric)
            // maximum; the rich side is limited by oxygen in GasSystem::react.
            // The upstream turbulence/dilution "mixing factor" keeps only its
            // cycle-to-cycle variation (randomness, used for sound), not its
            // mean reduction (low_efficiency_attenuation), which had no physical
            // basis (its dilution measure reads ~1.1 for fresh air).
            efficiencyAttenuation = 1.0 - mixingFactor * randomness * (1.0 - u);
        }
        else {
            const double rand_s =
                lowEfficiencyAttenuation
                * ((1 - randomness) + randomness * u);
            efficiencyAttenuation =
                (mixingFactor * rand_s + (1 - mixingFactor));
        }
        m_flameEvent.efficiency =
            efficiencyAttenuation * maxBurningEfficiency;
        // Two-zone density ratio: burned gas at the same pressure is hotter by
        // the heat of the burned fuel (real-gas u(T)); E = n_b T_b / (n_u T_u).
        m_flameEvent.massFractionBurned = 0.0;
        m_flameEvent.expansion = 1.0;
        if (combustion_physics::flameExpansion && m_system.n() > 0) {
            const double Tu = m_system.temperature();
            // Burned state: the fuel that the charge's oxygen can burn (the
            // flame is O2-limited on the rich side, GasSystem::react), and
            // the burned gas's own composition (products, real heat capacity).
            const GasSystem::Mix &mx = m_system.mix();
            const double o2PerFuel = 25.0 / 2.0;                      // GasSystem::react
            const double moleGrowth = (16.0 + 18.0) / (25.0 + 2.0);   // GasSystem::react
            const double reacted = std::min(mx.p_fuel, mx.p_o2 / o2PerFuel) * m_flameEvent.efficiency;
            const double q = reacted * m_fuel->getMolecularMass() * m_fuel->getEnergyDensity();
            const double reactants = reacted * (1.0 + o2PerFuel);
            const double nAfter = 1.0 + (moleGrowth - 1.0) * reactants;
            const double productsAfter = (mx.p_products + moleGrowth * reactants) / nAfter;
            const double ub = (m_system.kineticEnergy() / m_system.n() + q) / nAfter;
            const double Tb = GasSystem::temperatureFromEnergyPerMol(
                ub, m_system.degreesOfFreedom(), -1.0, productsAfter);
            // Volume ratio at equal pressure (per unit mass): E = n_b T_b / (n_u T_u).
            if (Tu > 0.0 && Tb > Tu) m_flameEvent.expansion = nAfter * Tb / Tu;
        }
        m_flameEvent.flameSpeed = m_fuel->flameSpeed(
            turbulence,
            afr,
            m_system.temperature(),
            m_system.pressure(),
            calculateFiringPressure(),
            units::pressure(160, units::psi));
    }
}

void CombustionChamber::beginCompressionIgnitionEvent(double fuelMass) {
    if (!m_engine->isCompressionIgnition()) return;
    m_lastInjectionTrappedAir = getTrappedAirMoles();
    m_lastInjectionTrappedO2 = m_system.n_o2();
    m_oxygenBudget = -1.0;
    m_engine->getCompressionIgnitionModel()->beginEvent(
        m_compressionIgnitionEvent,
        fuelMass,
        m_fuel->getMolecularMass(),
        m_engine->getSpeed());
    // Injection-hardware path: chamber length scale, piston turbulence (the
    // spark-ignition flame model's 0.5 x mean piston speed) and fuel density.
    m_compressionIgnitionEvent.lengthScale = m_head->getCylinderBank()->getBore() / 2.0;
    m_compressionIgnitionEvent.pistonTurbulence = 0.5 * std::abs(calculateMeanPistonSpeed());
    m_compressionIgnitionEvent.fuelDensity = m_fuel->getDensity();
    // Stoichiometric air/fuel mass ratio from the fuel's O2/fuel molar ratio
    // (air O2 fraction 0.2095, air molar mass 28.97 g/mol).
    m_compressionIgnitionEvent.stoichiometricAirFuel =
        m_fuel->getMolecularAfr() / 0.2095 * 0.02897 / std::max(1.0e-6, m_fuel->getMolecularMass());
    // Overall equivalence ratio of the event (ignition-delay correlation).
    if (m_lastInjectionTrappedO2 > 0.0 && m_fuel->getMolecularMass() > 0.0) {
        m_compressionIgnitionEvent.equivalenceRatio =
            (fuelMass / m_fuel->getMolecularMass()) * m_fuel->getMolecularAfr()
            / m_lastInjectionTrappedO2;
    }
}

void CombustionChamber::processCompressionIgnition(double dt) {
    if (!m_engine->isCompressionIgnition()) return;
    m_engine->recordCompressionIgnitionConditions(m_system.temperature(), m_system.pressure());
    const auto result = m_engine->getCompressionIgnitionModel()->step(
        m_compressionIgnitionEvent,
        dt,
        m_system.temperature(),
        m_system.pressure());

    if (result.fuelMolesToInject > 0.0) {
        m_system.injectFuel(result.fuelMolesToInject);
        m_engine->recordDirectInjectedFuelMass(
            result.fuelMolesToInject * m_fuel->getMolecularMass());
    }

    if (result.combustionStarted) {
        m_lit = true;
        m_litLastFrame = true;
    }

    // Diffusion (mixing-limited) diesel combustion cannot use all trapped
    // oxygen: fuel meeting already-depleted charge stays unburned (smoke).
    // The event may consume at most MaxOxygenUtilization of the oxygen
    // present when combustion starts. Source: the smoke-limited maximum
    // fuel/air equivalence ratio of direct-injection diesels, ~0.7-0.8
    // (Heywood, Internal Combustion Engine Fundamentals); 0.75 is its
    // midpoint. The governor's smoke limiter uses the same value.
    constexpr double MaxOxygenUtilization = 0.75;
    if (result.combustionStarted && m_oxygenBudget < 0.0) {
        m_oxygenBudget = MaxOxygenUtilization * m_system.n_o2();
    }

    double fuelMolesToBurn = result.fuelMolesToBurn;
    if (fuelMolesToBurn > 0.0 && m_oxygenBudget >= 0.0) {
        const double o2PerFuel = m_fuel->getMolecularAfr();
        fuelMolesToBurn = std::min(fuelMolesToBurn, m_oxygenBudget / std::max(o2PerFuel, 1.0e-9));
    }

    if (fuelMolesToBurn > 0.0) {
        const double before = m_system.pressure();
        const double reacted = m_system.reactFuel(
            fuelMolesToBurn,
            m_fuel->getMolecularAfr());
        if (m_oxygenBudget >= 0.0) {
            m_oxygenBudget = std::max(0.0, m_oxygenBudget - reacted * m_fuel->getMolecularAfr());
        }
        const double mass = reacted * m_fuel->getMolecularMass();
        m_system.changeEnergy(
            mass * m_fuel->getEnergyDensity() * m_fuel->getMaxBurningEfficiency());
        m_nBurntFuel += mass;
        m_engine->recordDirectBurnedFuelMass(mass);
        if (dt > 0.0) {
            m_combustionPressureRiseRate = std::max(
                m_combustionPressureRiseRate,
                std::max(0.0, (m_system.pressure() - before) / dt));
        }
    }

    if (!result.active) m_lit = false;
}

void CombustionChamber::update(double dt) {
    m_system.setVolume(getVolume());

    updateCycleStates();

    if (m_engine->isCompressionIgnition() || combustion_physics::unifiedHeatTransfer) {
        const double pistonSpeed = std::abs(calculateMeanPistonSpeed());
        m_heatTransferStepFactor = 130.0
            * std::pow(std::max(getVolume(), 1.0e-6), -0.06)
            * std::pow(pistonSpeed + 1.4, 0.8);
    }

    m_intakeFlowRate = m_head->intakeFlowRate(m_piston->getCylinderIndex());
    m_exhaustFlowRate = m_head->exhaustFlowRate(m_piston->getCylinderIndex());
    const bool exhaustValveIsOpen = m_exhaustFlowRate > 0.0;
    if (exhaustValveIsOpen && !m_exhaustValveWasOpen) {
        m_exhaustValveOpeningPressure = m_system.pressure();
        m_exhaustValveOpeningTemperature = m_system.temperature();
    }
    m_exhaustValveWasOpen = exhaustValveIsOpen;
}

void CombustionChamber::flow(double dt) {
    processCompressionIgnition(dt);

    if (m_system.temperature() > m_peakTemperature) {
        m_peakTemperature = m_system.temperature();
    }

    const double volume = getVolume();
    const double cylinderHeight = volume / m_cylinderCrossSectionSurfaceArea;
    // Exposed liner + fire deck + piston crown; the last two scale with the
    // chamber's surface relative to the flat bore area (ChamberAreaRatio).
    const double endArea = m_cylinderCrossSectionSurfaceArea * m_chamberAreaRatio;
    const double cylinderSurfaceArea =
        cylinderHeight * constants::pi * m_head->getCylinderBank()->getBore()
        + endArea * 2;

    double dT = units::celcius(90.0) - m_system.temperature();

    // Compression-ignition cylinders use the Hohenberg correlation for the
    // gas-to-wall heat-transfer coefficient (developed for direct-injection
    // diesels; scales with pressure, temperature, piston speed and size). The
    // original constant 100 W/m^2K is kept for spark-ignition engines.
    double heatTransferCoefficient = 100.0;
    if (m_engine->isCompressionIgnition() || combustion_physics::unifiedHeatTransfer) {
        const double pressureBar = std::max(0.01, m_system.pressure() / 1.0e5);
        const double temperature = std::max(1.0, m_system.temperature());
        // p^0.8 * T^-0.4 = exp(0.8 ln p - 0.4 ln T); the volume and piston
        // speed terms are per step (m_heatTransferStepFactor).
        heatTransferCoefficient = m_heatTransferStepFactor
            * std::exp(0.8 * std::log(pressureBar) - 0.4 * std::log(temperature));

        // The gas exchanges heat with the gas-side SURFACES, not the coolant:
        // area-weighted piston crown, cylinder head fire deck and exposed
        // liner. Typical full-load DI-diesel surface temperatures (Heywood,
        // ICE Fundamentals, ch. 12: piston crown ~300 C, head ~230 C, liner
        // ~150 C). The coolant-temperature wall (90 C) cooled the incoming
        // charge too little and overstated heat loss during combustion.
        // Engine inputs (defaults as above); e.g. air-cooled or optical
        // engines differ.
        const double PistonCrownTemperature = m_pistonWallTemperature;
        const double HeadTemperature = m_headWallTemperature;
        const double LinerTemperature = m_linerWallTemperature;
        const double linerArea = cylinderHeight * constants::pi * m_head->getCylinderBank()->getBore();
        const double wallTemperature =
            (endArea * (PistonCrownTemperature + HeadTemperature)
                + linerArea * LinerTemperature) / cylinderSurfaceArea;
        dT = wallTemperature - m_system.temperature();
    }

    const double wallHeat = dT * cylinderSurfaceArea * heatTransferCoefficient * dt;
    m_system.changeEnergy(wallHeat);
    m_heatLossTotal -= wallHeat;
    m_system.flow(m_piston->getBlowbyK(), dt, m_crankcasePressure, units::celcius(25.0));

    Intake *intake = m_head->getIntake(m_piston->getCylinderIndex());
    ExhaustSystem *exhaust = m_head->getExhaustSystem(m_piston->getCylinderIndex());

    const double start_n = m_system.n();

    GasSystem::FlowParameters flowParams;
    flowParams.dt = dt;

    flowParams.k_flow = m_manifoldToRunnerFlowRate;
    flowParams.crossSectionArea_0 = intake->getPlenumCrossSectionArea();
    flowParams.crossSectionArea_1 = m_head->getIntakeRunnerCrossSectionArea();
    flowParams.direction_x = 1.0;
    flowParams.direction_y = 0.0;
    flowParams.system_0 = &intake->m_system;
    flowParams.system_1 = &m_intakeRunnerAndManifold;
    if (combustion_physics::inertialRunners && !intake->hasForcedInductionFeed()) {
        // Runner gas column with inertia (Helmholtz / ram tuning). Losses:
        // entry 0.5 + pipe friction 0.02 L/D (textbook values).
        const double A = m_head->getIntakeRunnerCrossSectionArea();
        const double D = std::sqrt(4.0 * A / constants::pi);
        GasSystem::inertialFlow(&intake->m_system, &m_intakeRunnerAndManifold, m_intakeRunnerMassFlow,
            A, m_intakeRunnerLength, 0.5 + 0.02 * m_intakeRunnerLength / D, dt);
    }
    else {
        GasSystem::flow(flowParams);
    }

    m_intakeRunnerAndManifold.dissipateExcessVelocity();

    flowParams.k_flow = m_intakeFlowRate;
    flowParams.crossSectionArea_0 = m_head->getIntakeRunnerCrossSectionArea();
    flowParams.crossSectionArea_1 = volume / cylinderHeight;
    flowParams.direction_x = 1.0;
    flowParams.direction_y = 0.0;
    flowParams.system_0 = &m_intakeRunnerAndManifold;
    flowParams.system_1 = &m_system;
    const double intakeFlow = GasSystem::flow(flowParams);

    m_intakeRunnerAndManifold.dissipateExcessVelocity();
    m_system.dissipateExcessVelocity();

    flowParams.k_flow = m_exhaustFlowRate;
    flowParams.crossSectionArea_0 = volume / cylinderHeight;
    flowParams.crossSectionArea_1 = m_head->getExhaustRunnerCrossSectionArea();
    flowParams.direction_x = 1.0;
    flowParams.direction_y = 0.0;
    flowParams.system_0 = &m_system;
    flowParams.system_1 = &m_exhaustRunnerAndPrimary;
    const double exhaustFlow = GasSystem::flow(flowParams);

    m_system.dissipateExcessVelocity();
    m_exhaustRunnerAndPrimary.dissipateExcessVelocity();

    flowParams.k_flow = m_primaryToCollectorFlowRate;
    flowParams.crossSectionArea_0 = m_head->getExhaustRunnerCrossSectionArea();
    flowParams.crossSectionArea_1 = exhaust->getCollectorCrossSectionArea();
    flowParams.direction_x = 1.0;
    flowParams.direction_y = 0.0;
    flowParams.system_0 = &m_exhaustRunnerAndPrimary;
    // Naturally aspirated engines retain the original runner -> ExhaustSystem
    // connection. A configured TurboGroup returns its distinct pre-turbine
    // scroll instead; ExhaustSystem::m_system therefore stays post-turbine.
    flowParams.system_1 = m_engine->getExhaustDestination(exhaust);
    if (combustion_physics::inertialRunners && combustion_physics::inertialExhaust) {
        // Exhaust primary with inertia (pulse / scavenging dynamics). Losses:
        // exit 1.0 + pipe friction 0.02 L/D.
        const double A = m_head->getExhaustRunnerCrossSectionArea();
        const double D = std::sqrt(4.0 * A / constants::pi);
        GasSystem::inertialFlow(&m_exhaustRunnerAndPrimary, flowParams.system_1, m_exhaustRunnerMassFlow,
            A, m_exhaustRunnerLength, 1.0 + 0.02 * m_exhaustRunnerLength / D, dt);
    }
    else {
        GasSystem::flow(flowParams);
    }

    m_intakeRunnerAndManifold.updateVelocity(dt, intake->getVelocityDecay());
    m_system.updateVelocity(dt, 0.5);
    m_exhaustRunnerAndPrimary.updateVelocity(dt, exhaust->getVelocityDecay());
    m_exhaustRunnerPeakPressure = std::max(
        m_exhaustRunnerPeakPressure,
        m_exhaustRunnerAndPrimary.pressure());
    m_exhaustRunnerPeakTemperature = std::max(
        m_exhaustRunnerPeakTemperature,
        m_exhaustRunnerAndPrimary.temperature());

    if (!m_engine->isCompressionIgnition() && std::abs(intakeFlow) > 1E-9 && m_lit) {
        m_lit = false;
    }

    m_exhaustFlow = exhaustFlow;
    m_lastTimestepTotalExhaustFlow += exhaustFlow;
    m_lastTimestepTotalIntakeFlow += intakeFlow;

    if (m_lit && !m_engine->isCompressionIgnition()) {
        CylinderBank *bank = m_head->getCylinderBank();
        const double totalTravel_x = bank->getBore() / 2;
        const double totalTravel_y = volume / bank->boreSurfaceArea();
        const double expansion = volume / m_flameEvent.lastVolume;
        const double lastTravel_x = m_flameEvent.travel_x;
        const double lastTravel_y = m_flameEvent.travel_y * expansion;

        // Burned volume of the cylindrical flame (radius r, height h).
        auto burned = [&](double r, double h) {
            const double rc = std::fmin(r, totalTravel_x);
            return rc * rc * constants::pi * std::fmin(h, totalTravel_y);
        };

        double n = 0.0;
        double litVolume = 0.0;
        bool burning = false;
        if (combustion_physics::flameExpansion) {
            // Quasi-dimensional two-zone burning (Heywood, ICE Fundamentals,
            // sec. 14.4). The front moves at S_T into the unburned gas, so the
            // entrained mass is rho_u x (volume swept by that displacement).
            // The burned gas then fills y = E x / (1 + (E - 1) x) of the
            // chamber (two zones at one pressure, E = rho_u / rho_b), which
            // places the front. Driving the front at E x S_T relative to the
            // walls overstated the late burning rate by up to E, because the
            // rising pressure compresses the burned gas behind the front.
            const double E = m_flameEvent.expansion;
            const double S = m_flameEvent.flameSpeed;
            const double r0 = std::fmin(lastTravel_x, totalTravel_x);
            const double h0 = std::fmin(lastTravel_y, totalTravel_y);
            const double prevBurnedVolume = burned(r0, h0);
            const double swept = std::max(0.0, burned(r0 + dt * S, h0 + dt * S) - prevBurnedVolume);
            const double x0 = m_flameEvent.massFractionBurned;
            const double y0 = std::min(1.0, prevBurnedVolume / volume);
            const double unburnedVolume = std::max(volume * (1.0 - y0), 1.0e-12);
            double dx = 0.0;
            if (m_flameEvent.total_n > 0.0) {
                dx = std::fmin(1.0 - x0, (1.0 - x0) * swept / unburnedVolume);
            }
            const double x1 = x0 + dx;
            const double y1 = std::min(1.0, E * x1 / (1.0 + (E - 1.0) * x1));

            // Equal advance d of radius and height giving burned volume y1 V.
            double lo = 0.0, hi = totalTravel_x + totalTravel_y;
            for (int it = 0; it < 40; ++it) {
                const double mid = 0.5 * (lo + hi);
                if (burned(r0 + mid, h0 + mid) < y1 * volume) lo = mid; else hi = mid;
            }
            m_flameEvent.travel_x = std::fmin(r0 + hi, totalTravel_x);
            m_flameEvent.travel_y = std::fmin(h0 + hi, totalTravel_y);
            m_flameEvent.massFractionBurned = x1;

            n = dx * m_flameEvent.total_n;
            litVolume = burned(m_flameEvent.travel_x, m_flameEvent.travel_y) - prevBurnedVolume;
            burning = dx > 1.0e-12;
        }
        else {
            // Upstream: front at S_T, burned mass = burned volume fraction.
            const double flameSpeed = m_flameEvent.flameSpeed;
            m_flameEvent.travel_x =
                std::fmin(lastTravel_x + dt * flameSpeed, totalTravel_x);
            m_flameEvent.travel_y =
                std::fmin(lastTravel_y + dt * flameSpeed, totalTravel_y);
            burning = lastTravel_x < m_flameEvent.travel_x || lastTravel_y < m_flameEvent.travel_y;
            if (burning) {
                const double burnedVolume =
                    m_flameEvent.travel_x * m_flameEvent.travel_x
                    * constants::pi * m_flameEvent.travel_y;
                const double prevBurnedVolume =
                    lastTravel_x * lastTravel_x * constants::pi * lastTravel_y;
                litVolume = burnedVolume - prevBurnedVolume;
                n = (litVolume / volume) * m_system.n();
            }
        }

        if (burning) {
            const double fuelBurned =
                m_system.react(n * m_flameEvent.efficiency, m_flameEvent.globalMix);
            const double massFuelBurned = fuelBurned * m_fuel->getMolecularMass();
            m_system.changeEnergy(
                massFuelBurned * m_fuel->getEnergyDensity());

            m_flameEvent.lit_n += n;
            m_flameEvent.percentageLit += litVolume / volume;

            m_nBurntFuel += massFuelBurned;
        }
        else {
            m_lit = false;
        }

        m_flameEvent.lastVolume = volume;
    }
}

double CombustionChamber::lastEventAfr() const {
    const double totalFuel = m_flameEvent.globalMix.p_fuel * m_flameEvent.total_n;
    const double totalOxygen = m_flameEvent.globalMix.p_o2 * m_flameEvent.total_n;
    const double totalInert = m_flameEvent.globalMix.p_inert * m_flameEvent.total_n;

    constexpr double octaneMolarMass = units::mass(114.23, units::g);
    constexpr double oxygenMolarMass = units::mass(31.9988, units::g);
    constexpr double nitrogenMolarMass = units::mass(28.014, units::g);

    if (totalFuel == 0) return 0;
    else {
        return
            (oxygenMolarMass * totalOxygen + totalInert * nitrogenMolarMass)
            / (totalFuel * octaneMolarMass);
    }
}

// Spark-ignition chambers use the same heat transfer as compression ignition
// (Hohenberg, gas-side surfaces). Diagnostic switch; false = upstream
// 100 W/m^2K to a 90 C wall.
bool combustion_physics::unifiedHeatTransfer = true;
// Diagnostic switch (default true): the spark-ignition flame front grows with
// the burned-gas volume (density ratio T_b/T_u); false = upstream (burned
// volume fraction taken as mass fraction).
bool combustion_physics::flameExpansion = true;
// Diagnostic switch (default true): burning efficiency without the upstream
// mean attenuation (see CombustionChamber::ignite); false = upstream.
bool combustion_physics::unbiasedBurnEfficiency = true;
// Diagnostic switch (default true): intake runners and exhaust primaries are
// inertial pipes (gas column accelerated by the pressure difference, with
// entry/exit and friction losses); false = upstream quasi-steady orifices.
bool combustion_physics::inertialRunners = true;
bool combustion_physics::inertialExhaust = true;
double combustion_physics::pipeCavityShare = 0.0;

double CombustionChamber::calculateFrictionForce(double v_s) const {
    // The component friction model (Engine::getFrictionModel) includes the
    // piston and ring friction; the side-thrust model is then not applied.
    if (m_engine->usesComponentFriction()) return 0.0;
    const double cylinderWallForce = m_piston->calculateCylinderWallForce();

    const double F_coul = m_frictionModel.frictionCoeff * cylinderWallForce;
    const double v_st = m_frictionModel.breakawayFrictionVelocity * constants::root_2;
    const double v_coul = m_frictionModel.breakawayFrictionVelocity / 10;
    const double F_brk = m_frictionModel.breakawayFriction;
    const double v = std::abs(v_s);

    const double F_0 = constants::root_2 * constants::e * (F_brk - F_coul);
    const double F_1 = v / v_st;
    const double F_2 = std::exp(-F_1 * F_1) * F_1;
    const double F_3 = F_coul * std::tanh(v / v_coul);
    const double F_4 = m_frictionModel.viscousFrictionCoefficient * v;

    return F_0 * F_2 + F_3 + F_4;
}

void CombustionChamber::updateCycleStates() {
    double crankAngle = m_engine->getOutputCrankshaft()->getCycleAngle();
    if (std::isnan(crankAngle) || std::isinf(crankAngle)) {
        crankAngle = 0.0;
    }

    const int i = (int)std::round((crankAngle / (4 * constants::pi)) * (StateSamples - 1.0));

    m_pistonSpeed[i] = std::abs(pistonSpeed());
    m_pressure[i] = m_system.pressure();
}

void CombustionChamber::apply(atg_scs::SystemState *system) {
    CylinderBank *bank = m_head->getCylinderBank();
    const double area = (bank->getBore() * bank->getBore() / 4.0) * constants::pi;
    const double v_x = system->v_x[m_piston->m_body.index];
    const double v_y = system->v_y[m_piston->m_body.index];

    const double v_s =
        v_x * bank->getDx() + v_y * bank->getDy();

    const double pressureDifferential = m_system.pressure() - m_crankcasePressure;
    const double force = -area * pressureDifferential;

    if (std::isnan(force) || std::isinf(force)) {
        assert(false);
    }

    constexpr double limit = 1E-3;
    const double abs_v_s = std::fmin(std::abs(v_s), limit);
    const double attenuation = abs_v_s / limit;

    const double F = calculateFrictionForce(v_s) * attenuation;
    const double F_fric = (v_s > 0)
        ? -F
        : F;

    system->applyForce(
        0.0,
        0.0,
        (force + F_fric) * bank->getDx(),
        (force + F_fric) * bank->getDy(),
        m_piston->m_body.index);
}

double CombustionChamber::getFrictionForce() const {
    CylinderBank *bank = m_head->getCylinderBank();
    const double v_x = m_piston->m_body.v_x;
    const double v_y = m_piston->m_body.v_y;

    const double v_s =
        v_x * bank->getDx() + v_y * bank->getDy();

    return calculateFrictionForce(v_s);
}

