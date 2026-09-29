#include "../include/forced_induction_system.h"

#include "../include/engine.h"
#include "../include/exhaust_system.h"
#include "../include/intake.h"
#include "../include/units.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
constexpr double AmbientPressure = 101325.0;
constexpr double AmbientTemperature = 298.15;
constexpr double MinimumTemperature = 1.0;
constexpr double Gamma = 1.4;

double clamp01(double value) {
    return std::max(0.0, std::min(1.0, value));
}

GasSystem::Mix airMix() {
    GasSystem::Mix mix;
    mix.p_fuel = 0.0;
    mix.p_inert = 0.75;
    mix.p_o2 = 0.25;
    return mix;
}

double boundedTransfer(
    GasSystem &source,
    GasSystem &destination,
    double requestedMoles,
    double destinationTemperature)
{
    const double dn = std::max(0.0, std::min(requestedMoles, source.n()));
    if (dn <= 0.0) return 0.0;

    const GasSystem::Mix mix = source.mix();
    source.loseN(dn, source.kineticEnergyPerMol());
    destination.gainN(
        dn,
        GasSystem::kineticEnergyPerMol(
            std::max(MinimumTemperature, destinationTemperature),
            destination.degreesOfFreedom()),
        mix);
    return dn;
}
}

TurboGroup::TurboGroup() = default;

void TurboGroup::initialize(const Parameters &parameters) {
    m_parameters = parameters;
    m_parameters.inletChannelCount = std::max(1, m_parameters.inletChannelCount);
    m_parameters.preTurbineVolume = std::max(1.0e-6, m_parameters.preTurbineVolume);
    m_parameters.preTurbineArea = std::max(1.0e-6, m_parameters.preTurbineArea);
    m_parameters.compressorInletVolume = std::max(1.0e-6, m_parameters.compressorInletVolume);
    m_parameters.compressorDischargeVolume = std::max(1.0e-6, m_parameters.compressorDischargeVolume);
    m_parameters.coolerVolume = std::max(1.0e-6, m_parameters.coolerVolume);
    m_parameters.chargePlenumVolume = std::max(1.0e-6, m_parameters.chargePlenumVolume);
    m_parameters.chargeArea = std::max(1.0e-6, m_parameters.chargeArea);
    m_parameters.aftercoolerEffectiveness = clamp01(m_parameters.aftercoolerEffectiveness);
    m_parameters.vgtMinFlowFactor = clamp01(m_parameters.vgtMinFlowFactor);

    TurbochargerModel::Parameters rotating;
    rotating.enabled = m_parameters.enabled;
    rotating.shaftInertia = m_parameters.shaftInertia;
    rotating.frictionTorque = m_parameters.frictionTorque;
    rotating.maxSpeed = m_parameters.maxSpeed;
    rotating.maxPressureRatio = m_parameters.maxPressureRatio;
    rotating.compressorEfficiency = m_parameters.compressorEfficiency;
    rotating.turbineEfficiency = m_parameters.turbineEfficiency;
    rotating.aftercoolerEffectiveness = 0.0;
    rotating.designMassFlow = m_parameters.designMassFlow;
    rotating.turbineDesignPressureRatio = m_parameters.turbineDesignPressureRatio;
    rotating.turbineDesignTemperature = m_parameters.turbineDesignTemperature;
    m_rotatingAssembly.initialize(rotating);

    const GasSystem::Mix mix = airMix();
    m_ambient.initialize(AmbientPressure, 1000.0, AmbientTemperature, mix);
    m_compressorInlet.initialize(AmbientPressure, m_parameters.compressorInletVolume, AmbientTemperature, mix);
    m_compressorDischarge.initialize(AmbientPressure, m_parameters.compressorDischargeVolume, AmbientTemperature, mix);
    m_cooler.initialize(AmbientPressure, m_parameters.coolerVolume, AmbientTemperature, mix);
    m_chargePlenum.initialize(AmbientPressure, m_parameters.chargePlenumVolume, AmbientTemperature, mix);

    const double preTurbineVolumePerScroll =
        m_parameters.preTurbineVolume / static_cast<double>(m_parameters.inletChannelCount);
    m_preTurbine.assign(static_cast<std::size_t>(m_parameters.inletChannelCount), GasSystem{});
    for (GasSystem &scroll : m_preTurbine) {
        scroll.initialize(AmbientPressure, preTurbineVolumePerScroll, AmbientTemperature, mix);
        const double width = std::sqrt(m_parameters.preTurbineArea);
        scroll.setGeometry(width, width, 1.0, 0.0);
    }

    const double designFlow = std::max(1.0e-6, m_parameters.designMassFlow);
    m_inletFlowK = flowCoefficient(
        m_parameters.inletFlowRate > 0.0 ? m_parameters.inletFlowRate : designFlow,
        AmbientPressure, 2500.0, AmbientTemperature);
    m_passiveCompressorFlowK = flowCoefficient(
        m_parameters.passiveCompressorFlowRate > 0.0
            ? m_parameters.passiveCompressorFlowRate : designFlow,
        AmbientPressure, 5000.0, AmbientTemperature);
    m_coolerFlowK = flowCoefficient(
        m_parameters.coolerFlowRate > 0.0 ? m_parameters.coolerFlowRate : designFlow,
        AmbientPressure * 1.5,
        std::max(2500.0, m_parameters.coolerPressureLoss),
        AmbientTemperature * 1.2);
    m_chargeFlowK = flowCoefficient(
        m_parameters.chargeFlowRate > 0.0 ? m_parameters.chargeFlowRate : designFlow,
        AmbientPressure * 1.5, 5000.0, AmbientTemperature * 1.1);
    m_turbineFlowK = flowCoefficient(
        m_parameters.turbineFlowRate > 0.0 ? m_parameters.turbineFlowRate : designFlow,
        AmbientPressure * std::max(1.001, m_parameters.turbineDesignPressureRatio),
        AmbientPressure * (std::max(1.001, m_parameters.turbineDesignPressureRatio) - 1.0),
        std::max(250.0, m_parameters.turbineDesignTemperature));
    m_wastegateFlowK = flowCoefficient(
        m_parameters.wastegateFlowRate > 0.0 ? m_parameters.wastegateFlowRate : designFlow,
        AmbientPressure * std::max(1.001, m_parameters.turbineDesignPressureRatio),
        AmbientPressure * (std::max(1.001, m_parameters.turbineDesignPressureRatio) - 1.0),
        std::max(250.0, m_parameters.turbineDesignTemperature));
    m_bypassFlowK = flowCoefficient(
        m_parameters.compressorBypassFlowRate > 0.0
            ? m_parameters.compressorBypassFlowRate : designFlow,
        AmbientPressure * 1.5, AmbientPressure * 0.5, AmbientTemperature * 1.2);

    m_wastegateState = m_parameters.wastegateEnabled
        ? clamp01(m_parameters.wastegatePosition) : 0.0;
    m_bypassState = m_parameters.compressorBypassEnabled
        ? clamp01(m_parameters.compressorBypassPosition) : 0.0;
    m_vgtState = m_parameters.vgtEnabled ? clamp01(m_parameters.vgtPosition) : 1.0;
    m_wastegateCommand = m_wastegateState;
    m_bypassCommand = m_bypassState;
    m_vgtCommand = m_vgtState;
    m_telemetry = Telemetry{};
    m_telemetry.preTurbinePressure.resize(m_preTurbine.size(), AmbientPressure);
    m_telemetry.preTurbineTemperature.resize(m_preTurbine.size(), AmbientTemperature);
}

GasSystem *TurboGroup::preTurbineSystem(int scrollIndex) {
    if (scrollIndex < 0 || scrollIndex >= static_cast<int>(m_preTurbine.size())) return nullptr;
    return &m_preTurbine[static_cast<std::size_t>(scrollIndex)];
}

const GasSystem *TurboGroup::preTurbineSystem(int scrollIndex) const {
    if (scrollIndex < 0 || scrollIndex >= static_cast<int>(m_preTurbine.size())) return nullptr;
    return &m_preTurbine[static_cast<std::size_t>(scrollIndex)];
}

void TurboGroup::setWastegateCommand(double command) {
    m_wastegateCommand = m_parameters.wastegateEnabled ? clamp01(command) : 0.0;
}

void TurboGroup::setCompressorBypassCommand(double command) {
    m_bypassCommand = m_parameters.compressorBypassEnabled ? clamp01(command) : 0.0;
}

void TurboGroup::setVgtCommand(double command) {
    m_vgtCommand = m_parameters.vgtEnabled ? clamp01(command) : 1.0;
}

double TurboGroup::flowCoefficient(
    double configuredMassFlow,
    double designPressure,
    double designPressureDrop,
    double designTemperature) const
{
    const double molarFlow = std::max(1.0e-9, configuredMassFlow) / units::AirMolecularMass;
    return GasSystem::flowConstant(
        molarFlow,
        std::max(1.0, designPressure),
        std::max(1.0, std::min(designPressure * 0.999, designPressureDrop)),
        std::max(MinimumTemperature, designTemperature),
        GasSystem::heatCapacityRatio(5));
}

double TurboGroup::updateActuator(
    double current,
    double command,
    double timeConstant,
    double dt) const
{
    const double target = clamp01(command);
    if (timeConstant <= 0.0) return target;
    const double a = 1.0 - std::exp(-std::max(0.0, dt) / timeConstant);
    return clamp01(current + (target - current) * a);
}

double TurboGroup::transferCompressedGas(double dt) {
    GasSystem::FlowParameters flow{};
    flow.dt = dt;
    flow.direction_x = 1.0;
    flow.direction_y = 0.0;
    flow.crossSectionArea_0 = m_parameters.chargeArea;
    flow.crossSectionArea_1 = m_parameters.chargeArea;
    flow.system_0 = &m_ambient;
    flow.system_1 = &m_compressorInlet;
    flow.k_flow = m_inletFlowK;
    GasSystem::flow(flow);

    double transferred = 0.0;
    if (m_rotatingAssembly.shaftSpeed() <= 1.0e-9) {
        // With a stationary shaft the compressor supplies no head or work,
        // but its configured passive path remains available for cranking.
        flow.system_0 = &m_compressorInlet;
        flow.system_1 = &m_compressorDischarge;
        flow.k_flow = m_passiveCompressorFlowK;
        transferred = std::max(0.0, GasSystem::flow(flow));
        m_telemetry.compressorPressureRatio = 1.0;
        m_telemetry.compressorPower = 0.0;
    }
    else {
        const auto point = m_rotatingAssembly.compressorOperatingPoint(
            m_compressorDischarge.pressure(),
            m_compressorInlet.pressure(),
            m_compressorInlet.temperature());
        const double requestedMoles = point.massFlow * dt / units::AirMolecularMass;
        transferred = boundedTransfer(
            m_compressorInlet,
            m_compressorDischarge,
            requestedMoles,
            point.compressorOutletTemperature);
        const double requestedMass = point.massFlow * dt;
        const double actualMass = transferred * units::AirMolecularMass;
        const double fraction = requestedMass > 0.0
            ? std::min(1.0, actualMass / requestedMass) : 0.0;
        m_telemetry.compressorPressureRatio = point.pressureRatio;
        m_telemetry.compressorPower = point.power * fraction;

        // A slowly turning wheel is still an open flow passage. When the
        // engine draws the discharge side below the inlet, air is sucked
        // through by that pressure difference in addition to the mapped
        // delivery. This path adds no work and no head, so it cannot raise
        // discharge pressure above the inlet.
        if (m_compressorDischarge.pressure() < m_compressorInlet.pressure()) {
            flow.system_0 = &m_compressorInlet;
            flow.system_1 = &m_compressorDischarge;
            flow.k_flow = m_passiveCompressorFlowK;
            transferred += std::max(0.0, GasSystem::flow(flow));
        }
    }

    m_telemetry.compressorMassFlow = transferred * units::AirMolecularMass / dt;
    return transferred;
}

double TurboGroup::processTurbineFlow(double dt, Engine &engine) {
    if (m_parameters.postTurbineExhaustIndex < 0
        || m_parameters.postTurbineExhaustIndex >= engine.getExhaustSystemCount()) {
        return 0.0;
    }

    GasSystem *post = engine.getExhaustSystem(m_parameters.postTurbineExhaustIndex)->getSystem();
    const double flowFactor = m_parameters.vgtEnabled
        ? m_parameters.vgtMinFlowFactor
            + (1.0 - m_parameters.vgtMinFlowFactor) * m_vgtState
        : 1.0;

    double turbineMoles = 0.0;
    double turbineEnergy = 0.0;
    double weightedPressureRatio = 0.0;
    for (GasSystem &scroll : m_preTurbine) {
        const double inletPressure = scroll.pressure();
        const double inletTemperature = scroll.temperature();
        const double outletPressure = post->pressure();
        const double pressureRatio = std::max(1.0, inletPressure / std::max(1.0, outletPressure));
        const double inletEnergyPerMol = scroll.kineticEnergyPerMol();

        GasSystem::FlowParameters flow{};
        flow.k_flow = m_turbineFlowK * flowFactor / static_cast<double>(m_preTurbine.size());
        flow.dt = dt;
        flow.direction_x = 1.0;
        flow.direction_y = 0.0;
        flow.crossSectionArea_0 = m_parameters.preTurbineArea;
        flow.crossSectionArea_1 = engine.getExhaustSystem(
            m_parameters.postTurbineExhaustIndex)->getCollectorCrossSectionArea();
        flow.system_0 = &scroll;
        flow.system_1 = post;
        const double moved = GasSystem::flow(flow);
        if (moved <= 0.0) continue;

        constexpr double exponent = (Gamma - 1.0) / Gamma;
        const double cpMolar = (Gamma / (Gamma - 1.0)) * 8.31446261815324;
        const double idealWorkPerMol = cpMolar
            * std::max(MinimumTemperature, inletTemperature)
            * (1.0 - std::pow(1.0 / pressureRatio, exponent));
        const double requestedEnergy = moved
            * std::max(0.0, idealWorkPerMol)
            * clamp01(m_parameters.turbineEfficiency);
        const double transferredEnergy = moved * std::max(0.0, inletEnergyPerMol);
        const double postMinimumEnergy = GasSystem::kineticEnergyPerMol(
            MinimumTemperature, post->degreesOfFreedom()) * post->n();
        const double postAvailable = std::max(0.0, post->kineticEnergy() - postMinimumEnergy);
        const double extracted = std::min(requestedEnergy, std::min(transferredEnergy, postAvailable));
        post->changeEnergy(-extracted);

        turbineMoles += moved;
        turbineEnergy += extracted;
        weightedPressureRatio += moved * pressureRatio;
    }

    m_telemetry.turbineMassFlow = turbineMoles * units::AirMolecularMass / dt;
    m_telemetry.turbinePower = turbineEnergy / dt;
    m_telemetry.turbinePressureRatio = turbineMoles > 0.0
        ? weightedPressureRatio / turbineMoles : 1.0;
    return turbineEnergy;
}

double TurboGroup::processWastegateFlow(double dt, Engine &engine) {
    m_telemetry.wastegateMassFlow = 0.0;
    if (!m_parameters.wastegateEnabled || m_wastegateState <= 0.0
        || m_parameters.postTurbineExhaustIndex < 0
        || m_parameters.postTurbineExhaustIndex >= engine.getExhaustSystemCount()) {
        return 0.0;
    }

    GasSystem *post = engine.getExhaustSystem(m_parameters.postTurbineExhaustIndex)->getSystem();
    double movedMoles = 0.0;
    for (GasSystem &scroll : m_preTurbine) {
        GasSystem::FlowParameters flow{};
        flow.k_flow = m_wastegateFlowK * m_wastegateState
            / static_cast<double>(m_preTurbine.size());
        flow.dt = dt;
        flow.direction_x = 1.0;
        flow.direction_y = 0.0;
        flow.crossSectionArea_0 = m_parameters.preTurbineArea;
        flow.crossSectionArea_1 = engine.getExhaustSystem(
            m_parameters.postTurbineExhaustIndex)->getCollectorCrossSectionArea();
        flow.system_0 = &scroll;
        flow.system_1 = post;
        movedMoles += std::max(0.0, GasSystem::flow(flow));
    }
    m_telemetry.wastegateMassFlow = movedMoles * units::AirMolecularMass / dt;
    return movedMoles;
}

double TurboGroup::processChargePath(double dt, Engine &engine) {
    GasSystem::FlowParameters flow{};
    flow.k_flow = m_coolerFlowK;
    flow.dt = dt;
    flow.direction_x = 1.0;
    flow.direction_y = 0.0;
    flow.crossSectionArea_0 = m_parameters.chargeArea;
    flow.crossSectionArea_1 = m_parameters.chargeArea;
    GasSystem *chargeSource = &m_compressorDischarge;
    if (m_parameters.chargeAirCoolerEnabled) {
        flow.system_0 = &m_compressorDischarge;
        flow.system_1 = &m_cooler;
        const double coolerInflow = GasSystem::flow(flow);
        if (coolerInflow > 0.0) {
            const double targetTemperature = AmbientTemperature
                + (m_cooler.temperature() - AmbientTemperature)
                * (1.0 - m_parameters.aftercoolerEffectiveness);
            const double removable = coolerInflow
                * std::max(0.0, m_cooler.kineticEnergyPerMol()
                    - GasSystem::kineticEnergyPerMol(
                        targetTemperature, m_cooler.degreesOfFreedom()));
            const double minimumEnergy = GasSystem::kineticEnergyPerMol(
                MinimumTemperature, m_cooler.degreesOfFreedom()) * m_cooler.n();
            m_cooler.changeEnergy(-std::min(
                removable,
                std::max(0.0, m_cooler.kineticEnergy() - minimumEnergy)));
        }
        chargeSource = &m_cooler;
    }

    // The optional air-control valve is upstream of the charge plenum, as
    // required by the locked topology. Existing engine throttle state is only
    // an actuator command here; fuel handling remains outside this subsystem.
    double throttleFactor = 1.0;
    if (m_parameters.throttleEnabled) {
        for (int intakeIndex : m_parameters.intakeIndices) {
            if (intakeIndex < 0 || intakeIndex >= engine.getIntakeCount()) continue;
            throttleFactor = std::cos(
                clamp01(engine.getIntake(intakeIndex)->getThrottlePlatePosition())
                * 3.14159265358979323846 / 2.0);
            break;
        }
    }

    flow.system_0 = chargeSource;
    flow.system_1 = &m_chargePlenum;
    flow.k_flow = m_chargeFlowK * throttleFactor;
    GasSystem::flow(flow);

    // Downstream of the charge plenum, retain the existing intake
    // plenum/runner/valve model without another throttle restriction.
    double intakeMoles = 0.0;
    const int routeCount = static_cast<int>(m_parameters.intakeIndices.size());
    for (int intakeIndex : m_parameters.intakeIndices) {
        if (intakeIndex < 0 || intakeIndex >= engine.getIntakeCount()) continue;
        Intake *intake = engine.getIntake(intakeIndex);
        flow.system_0 = &m_chargePlenum;
        flow.system_1 = intake->getSystem();
        flow.k_flow = m_chargeFlowK / static_cast<double>(std::max(1, routeCount));
        flow.crossSectionArea_1 = intake->getPlenumCrossSectionArea();
        const double moved = GasSystem::flow(flow);
        if (moved > 0.0) {
            intakeMoles += moved;
            intake->recordForcedInductionAir(moved);
        }
    }

    m_telemetry.intakeMassFlow = intakeMoles * units::AirMolecularMass / dt;
    return intakeMoles;
}

double TurboGroup::processBypass(double dt) {
    m_telemetry.bypassMassFlow = 0.0;
    if (!m_parameters.compressorBypassEnabled || m_bypassState <= 0.0) return 0.0;

    GasSystem::FlowParameters flow{};
    flow.k_flow = m_bypassFlowK * m_bypassState;
    flow.dt = dt;
    flow.direction_x = -1.0;
    flow.direction_y = 0.0;
    flow.crossSectionArea_0 = m_parameters.chargeArea;
    flow.crossSectionArea_1 = m_parameters.chargeArea;
    flow.system_0 = m_parameters.compressorBypassRecirculates
        ? &m_compressorInlet : &m_ambient;
    flow.system_1 = &m_chargePlenum;
    const double moved = GasSystem::flow(flow);
    const double bypassMoles = std::max(0.0, -moved);
    m_telemetry.bypassMassFlow = bypassMoles * units::AirMolecularMass / dt;
    return bypassMoles;
}

void TurboGroup::updateTelemetry(Engine &engine) {
    for (std::size_t i = 0; i < m_preTurbine.size(); ++i) {
        m_telemetry.preTurbinePressure[i] = m_preTurbine[i].pressure();
        m_telemetry.preTurbineTemperature[i] = m_preTurbine[i].temperature();
    }
    if (m_parameters.postTurbineExhaustIndex >= 0
        && m_parameters.postTurbineExhaustIndex < engine.getExhaustSystemCount()) {
        GasSystem *post = engine.getExhaustSystem(m_parameters.postTurbineExhaustIndex)->getSystem();
        m_telemetry.postTurbinePressure = post->pressure();
        m_telemetry.postTurbineTemperature = post->temperature();
    }
    m_telemetry.shaftSpeed = m_rotatingAssembly.shaftSpeed();
    m_telemetry.compressorDischargeTemperature = m_compressorDischarge.temperature();
    m_telemetry.coolerInletPressure = m_compressorDischarge.pressure();
    m_telemetry.coolerInletTemperature = m_compressorDischarge.temperature();
    const GasSystem &coolerOutlet = m_parameters.chargeAirCoolerEnabled
        ? m_cooler : m_compressorDischarge;
    m_telemetry.coolerOutletPressure = coolerOutlet.pressure();
    m_telemetry.coolerOutletTemperature = coolerOutlet.temperature();
    m_telemetry.chargePlenumPressure = m_chargePlenum.pressure();
    m_telemetry.chargePlenumTemperature = m_chargePlenum.temperature();
    m_telemetry.wastegatePosition = m_wastegateState;
    m_telemetry.compressorBypassPosition = m_bypassState;
    m_telemetry.vgtPosition = m_vgtState;
}

void TurboGroup::process(double dt, Engine &engine) {
    if (!enabled() || dt <= 0.0) return;

    m_wastegateState = updateActuator(
        m_wastegateState, m_wastegateCommand,
        m_parameters.wastegateTimeConstant, dt);
    m_bypassState = updateActuator(
        m_bypassState, m_bypassCommand,
        m_parameters.compressorBypassTimeConstant, dt);
    m_vgtState = updateActuator(
        m_vgtState, m_vgtCommand,
        m_parameters.vgtTimeConstant, dt);

    m_ambient.reset(AmbientPressure, AmbientTemperature, airMix());
    transferCompressedGas(dt);
    processChargePath(dt, engine);
    processBypass(dt);
    processWastegateFlow(dt, engine);
    processTurbineFlow(dt, engine);

    m_rotatingAssembly.advanceShaft(
        dt,
        m_telemetry.turbinePower,
        m_telemetry.compressorPower);

    // These are lumped, well-mixed volumes: a jet entering one mixes out
    // within the sub-step. Without this, bulk momentum persisted near Mach 1
    // (no velocity decay was ever applied here), static temperature fell
    // below ambient and each volume rammed the next, so pressure rose along
    // the flow direction and the turbo loop ran on zero fuel. The kinetic
    // energy becomes internal energy, so total energy is conserved.
    m_compressorInlet.dissipateVelocity(dt, 0.0);
    m_compressorDischarge.dissipateVelocity(dt, 0.0);
    m_cooler.dissipateVelocity(dt, 0.0);
    m_chargePlenum.dissipateVelocity(dt, 0.0);
    for (GasSystem &scroll : m_preTurbine) scroll.dissipateVelocity(dt, 0.0);
    updateTelemetry(engine);
}

void ForcedInductionSystem::initialize(
    const Parameters &parameters,
    int exhaustSystemCount,
    int intakeCount)
{
    m_groups.clear();
    m_exhaustRoutes.assign(static_cast<std::size_t>(std::max(0, exhaustSystemCount)), ExhaustRoute{});
    m_intakeRoutes.assign(static_cast<std::size_t>(std::max(0, intakeCount)), -1);

    for (std::size_t groupIndex = 0; groupIndex < parameters.groups.size(); ++groupIndex) {
        TurboGroup::Parameters groupParameters = parameters.groups[groupIndex];
        if (!groupParameters.enabled) continue;

        if (groupParameters.exhaustSystemIndices.empty()) {
            for (int i = 0; i < exhaustSystemCount; ++i) groupParameters.exhaustSystemIndices.push_back(i);
        }
        if (groupParameters.intakeIndices.empty()) {
            for (int i = 0; i < intakeCount; ++i) groupParameters.intakeIndices.push_back(i);
        }
        if (groupParameters.postTurbineExhaustIndex < 0
            && !groupParameters.exhaustSystemIndices.empty()) {
            groupParameters.postTurbineExhaustIndex = groupParameters.exhaustSystemIndices.front();
        }
        if (groupParameters.exhaustScrollIndices.empty()) {
            groupParameters.exhaustScrollIndices.assign(
                groupParameters.exhaustSystemIndices.size(), 0);
        }
        if (groupParameters.exhaustScrollIndices.size()
            != groupParameters.exhaustSystemIndices.size()) {
            throw std::invalid_argument("TurboGroup exhaust route/scroll counts differ");
        }

        const int storedGroupIndex = static_cast<int>(m_groups.size());
        for (std::size_t i = 0; i < groupParameters.exhaustSystemIndices.size(); ++i) {
            const int exhaustIndex = groupParameters.exhaustSystemIndices[i];
            const int scrollIndex = groupParameters.exhaustScrollIndices[i];
            if (exhaustIndex < 0 || exhaustIndex >= exhaustSystemCount
                || scrollIndex < 0 || scrollIndex >= groupParameters.inletChannelCount) {
                throw std::out_of_range("TurboGroup exhaust route is outside configured engine geometry");
            }
            if (m_exhaustRoutes[static_cast<std::size_t>(exhaustIndex)].group >= 0) {
                throw std::invalid_argument("An exhaust system cannot feed two TurboGroups without a routing valve");
            }
            m_exhaustRoutes[static_cast<std::size_t>(exhaustIndex)] = { storedGroupIndex, scrollIndex };
        }
        for (int intakeIndex : groupParameters.intakeIndices) {
            if (intakeIndex < 0 || intakeIndex >= intakeCount) {
                throw std::out_of_range("TurboGroup intake route is outside configured engine geometry");
            }
            // Multiple groups may intentionally share the same downstream
            // intake volume (parallel compressors feeding a common plenum).
            m_intakeRoutes[static_cast<std::size_t>(intakeIndex)] = storedGroupIndex;
        }

        TurboGroup group;
        group.initialize(groupParameters);
        m_groups.push_back(group);
    }
}

TurboGroup *ForcedInductionSystem::group(std::size_t index) {
    return index < m_groups.size() ? &m_groups[index] : nullptr;
}

const TurboGroup *ForcedInductionSystem::group(std::size_t index) const {
    return index < m_groups.size() ? &m_groups[index] : nullptr;
}

bool ForcedInductionSystem::managesIntake(int intakeIndex) const {
    return intakeIndex >= 0
        && intakeIndex < static_cast<int>(m_intakeRoutes.size())
        && m_intakeRoutes[static_cast<std::size_t>(intakeIndex)] >= 0;
}

GasSystem *ForcedInductionSystem::exhaustDestination(
    int exhaustSystemIndex,
    GasSystem *naturallyAspiratedDestination)
{
    if (exhaustSystemIndex < 0
        || exhaustSystemIndex >= static_cast<int>(m_exhaustRoutes.size())) {
        return naturallyAspiratedDestination;
    }
    const ExhaustRoute route = m_exhaustRoutes[static_cast<std::size_t>(exhaustSystemIndex)];
    if (route.group < 0 || route.group >= static_cast<int>(m_groups.size())) {
        return naturallyAspiratedDestination;
    }
    GasSystem *destination = m_groups[static_cast<std::size_t>(route.group)]
        .preTurbineSystem(route.scroll);
    return destination != nullptr ? destination : naturallyAspiratedDestination;
}

void ForcedInductionSystem::process(double dt, Engine &engine) {
    for (TurboGroup &groupState : m_groups) {
        groupState.process(dt, engine);
    }
}
