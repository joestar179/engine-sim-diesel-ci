#include "../include/turbocharger_model.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr double Gamma = 1.4;
constexpr double GasConstant = 8.31446261815324;
constexpr double AirMolecularMass = 0.02897;
constexpr double CompressorMapFlowCoeff = 0.18;
constexpr double MaxCompressorFlowRatio = 1.50;

inline double clamp01(double x) {
    return std::max(0.0, std::min(1.0, x));
}
}

TurbochargerModel::TurbochargerModel() = default;

void TurbochargerModel::initialize(const Parameters &parameters) {
    m_parameters = parameters;
    m_parameters.shaftInertia = std::max(m_parameters.shaftInertia, 1.0e-9);
    m_parameters.frictionTorque = std::max(0.0, m_parameters.frictionTorque);
    m_parameters.maxSpeed = std::max(m_parameters.maxSpeed, 1.0e-9);
    m_parameters.maxPressureRatio = std::max(1.0, m_parameters.maxPressureRatio);
    m_parameters.compressorEfficiency = std::max(0.05, std::min(1.0, m_parameters.compressorEfficiency));
    m_parameters.turbineEfficiency = clamp01(m_parameters.turbineEfficiency);
    m_parameters.aftercoolerEffectiveness = clamp01(m_parameters.aftercoolerEffectiveness);
    m_parameters.designMassFlow = std::max(m_parameters.designMassFlow, 1.0e-9);
    m_parameters.turbineDesignPressureRatio = std::max(1.001, m_parameters.turbineDesignPressureRatio);
    m_parameters.turbineDesignTemperature = std::max(250.0, m_parameters.turbineDesignTemperature);
    reset();
}

void TurbochargerModel::reset() { m_speed = 0.0; }

double TurbochargerModel::shaftEnergy() const {
    return 0.5 * m_parameters.shaftInertia * m_speed * m_speed;
}

void TurbochargerModel::advanceShaft(
    double dt,
    double turbinePower,
    double compressorPower)
{
    if (!enabled() || dt <= 0.0) return;

    // A non-zero reference speed gives a stationary turbine finite starting
    // torque while keeping the model bounded at zero shaft speed. Compressor
    // power is zero at rest because the compressor map produces no active flow.
    const double referenceSpeed = std::max(
        std::abs(m_speed),
        std::max(1.0, 0.01 * m_parameters.maxSpeed));
    const double turbineTorque = std::max(0.0, turbinePower) / referenceSpeed;
    const double compressorTorque = std::max(0.0, compressorPower) / referenceSpeed;
    // Journal-bearing and windage losses rise with speed. frictionTorque is
    // the loss torque at maximum speed; 30 % of it is speed independent
    // (breakaway/mixed friction) and 70 % scales linearly with speed. With
    // the ALCO 350B figures this keeps the free rundown from maximum speed in
    // the documented 90-180 s band (I*w_max/(0.7*T) * ln(1 + 0.7/0.3) = 165 s)
    // while no longer charging full-speed losses during low-speed spool-up.
    const double speedFraction = clamp01(std::abs(m_speed) / std::max(1.0e-9, m_parameters.maxSpeed));
    const double frictionTorque = m_speed > 0.0
        ? m_parameters.frictionTorque * (0.3 + 0.7 * speedFraction)
        : 0.0;
    const double acceleration =
        (turbineTorque - compressorTorque - frictionTorque)
        / m_parameters.shaftInertia;

    m_speed = std::max(0.0, std::min(
        m_parameters.maxSpeed,
        m_speed + acceleration * dt));
}

double TurbochargerModel::pressureRatioFor(double speed, double massFlow) const {
    if (!enabled()) return 1.0;
    const double speedRatio = clamp01(speed / m_parameters.maxSpeed);
    if (speedRatio <= 1.0e-9) return 1.0;
    const double flowCapacity = m_parameters.designMassFlow * speedRatio;
    const double flowRatio = std::max(0.0, massFlow) / std::max(flowCapacity, 1.0e-9);
    const double attenuation = 1.0 / (1.0 + CompressorMapFlowCoeff * flowRatio * flowRatio);
    return 1.0 + (m_parameters.maxPressureRatio - 1.0)
        * speedRatio * speedRatio * attenuation;
}

double TurbochargerModel::compressorOutletTemperature(double pressureRatio, double ambientTemperature) const {
    constexpr double exponent = (Gamma - 1.0) / Gamma;
    const double idealOutlet = ambientTemperature * std::pow(std::max(pressureRatio, 1.0), exponent);
    return ambientTemperature + (idealOutlet - ambientTemperature) / m_parameters.compressorEfficiency;
}

TurbochargerModel::CompressorPoint TurbochargerModel::compressorOperatingPoint(
    double downstreamPressure,
    double ambientPressure,
    double ambientTemperature,
    double flowShare) const
{
    CompressorPoint p;
    p.outletPressure = ambientPressure;
    p.compressorOutletTemperature = ambientTemperature;
    p.chargeTemperature = ambientTemperature;
    if (!enabled() || ambientPressure <= 0.0 || flowShare <= 0.0) return p;

    const double speedRatio = clamp01(m_speed / m_parameters.maxSpeed);
    if (speedRatio <= 1.0e-9) return p;
    const double maxHead = (m_parameters.maxPressureRatio - 1.0) * speedRatio * speedRatio;
    if (maxHead <= 1.0e-9) return p;

    const double requiredPr = std::max(1.0, downstreamPressure / ambientPressure);
    const double requiredHead = requiredPr - 1.0;
    if (requiredHead >= maxHead) {
        p.pressureRatio = 1.0 + maxHead;
        p.outletPressure = ambientPressure * p.pressureRatio;
        p.compressorOutletTemperature = compressorOutletTemperature(p.pressureRatio, ambientTemperature);
        p.chargeTemperature = ambientTemperature
            + (p.compressorOutletTemperature - ambientTemperature)
            * (1.0 - m_parameters.aftercoolerEffectiveness);
        return p;
    }

    double flowRatio = MaxCompressorFlowRatio;
    if (requiredHead > 1.0e-9) {
        const double a = std::max(1.0e-6, std::min(1.0, requiredHead / maxHead));
        flowRatio = std::sqrt(std::max(0.0, (1.0 / a - 1.0) / CompressorMapFlowCoeff));
        flowRatio = std::min(flowRatio, MaxCompressorFlowRatio);
    }

    const double totalMassFlow = m_parameters.designMassFlow * speedRatio * flowRatio;
    p.massFlow = totalMassFlow * std::min(1.0, flowShare);
    p.pressureRatio = pressureRatioFor(m_speed, totalMassFlow);
    p.outletPressure = ambientPressure * p.pressureRatio;
    p.compressorOutletTemperature = compressorOutletTemperature(p.pressureRatio, ambientTemperature);
    p.chargeTemperature = ambientTemperature
        + (p.compressorOutletTemperature - ambientTemperature)
        * (1.0 - m_parameters.aftercoolerEffectiveness);

    const double cp = (Gamma / (Gamma - 1.0)) * GasConstant / AirMolecularMass;
    p.power = p.massFlow * cp * std::max(0.0, p.compressorOutletTemperature - ambientTemperature);
    return p;
}

TurbochargerModel::Output TurbochargerModel::step(const Inputs &in) {
    Output out;
    out.shaftSpeed = m_speed;
    out.chargePressure = in.ambientPressure;
    out.chargeTemperature = in.ambientTemperature;
    if (!enabled() || in.dt <= 0.0) return out;

    const double cp = (Gamma / (Gamma - 1.0)) * GasConstant / AirMolecularMass;
    constexpr double exponent = (Gamma - 1.0) / Gamma;
    const double intakeMassFlow = std::max(0.0, in.intakeMassFlow);
    out.compressorPower = std::max(0.0, in.compressorPowerDemand);

    const double exhaustMassFlow = std::max(0.0, in.exhaustMassFlow);
    out.turbinePressureRatio = std::max(1.0, in.exhaustPressure / std::max(in.turbineOutletPressure, 1.0));
    const double turbineSpecificWork = cp
        * std::max(in.exhaustTemperature, in.ambientTemperature)
        * (1.0 - std::pow(1.0 / out.turbinePressureRatio, exponent))
        * m_parameters.turbineEfficiency;
    const double requested = exhaustMassFlow * std::max(0.0, turbineSpecificWork) * in.dt;
    out.extractedTurbineEnergy = std::min(requested, std::max(0.0, in.maxExtractableExhaustEnergy));
    out.rejectedTurbineEnergy = std::max(0.0, requested - out.extractedTurbineEnergy);
    out.turbinePower = out.extractedTurbineEnergy / in.dt;

    double nextEnergy = shaftEnergy()
        + out.extractedTurbineEnergy
        - out.compressorPower * in.dt
        - m_parameters.frictionTorque * std::abs(m_speed) * in.dt;
    nextEnergy = std::max(0.0, nextEnergy);
    const double maxEnergy = 0.5 * m_parameters.shaftInertia * m_parameters.maxSpeed * m_parameters.maxSpeed;
    nextEnergy = std::min(nextEnergy, maxEnergy);
    m_speed = std::sqrt(2.0 * nextEnergy / m_parameters.shaftInertia);

    out.shaftSpeed = m_speed;
    out.pressureRatio = pressureRatioFor(m_speed, intakeMassFlow);
    out.chargePressure = in.ambientPressure * out.pressureRatio;
    const double Tout = compressorOutletTemperature(out.pressureRatio, in.ambientTemperature);
    out.chargeTemperature = in.ambientTemperature
        + (Tout - in.ambientTemperature) * (1.0 - m_parameters.aftercoolerEffectiveness);
    return out;
}

