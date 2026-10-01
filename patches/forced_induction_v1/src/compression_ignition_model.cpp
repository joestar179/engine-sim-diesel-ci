#include "../include/compression_ignition_model.h"

#include <algorithm>
#include <cmath>

namespace {
double clamp01(double x) {
    return std::max(0.0, std::min(1.0, x));
}
}

CompressionIgnitionModel::CompressionIgnitionModel() = default;

void CompressionIgnitionModel::initialize(const Parameters &parameters) {
    m_parameters = parameters;
    m_parameters.premixedBurnFraction = clamp01(m_parameters.premixedBurnFraction);
    m_parameters.maxFuelMassPerCycle = std::max(0.0, m_parameters.maxFuelMassPerCycle);
    m_parameters.injectionDuration = std::max(0.0, m_parameters.injectionDuration);
    m_parameters.ignitionDelay = std::max(0.0, m_parameters.ignitionDelay);
    m_parameters.combustionDuration = std::max(0.0, m_parameters.combustionDuration);
}

void CompressionIgnitionModel::beginEvent(
    Event &event,
    double fuelMass,
    double fuelMolecularMass,
    double engineSpeedRadPerSec) const
{
    event = Event{};
    if (!enabled() || fuelMass <= 0.0 || fuelMolecularMass <= 0.0) return;

    const double omega = std::max(std::abs(engineSpeedRadPerSec), 1.0);
    event.active = true;
    event.fuelMolecularMass = fuelMolecularMass;
    event.maxDuration = 2.0 * 3.14159265358979 / omega;   // one revolution
    event.targetFuelMoles = fuelMass / fuelMolecularMass;
    event.injectionDurationSeconds = std::max(m_parameters.injectionDuration / omega, 1.0e-6);
    event.ignitionDelaySeconds = m_parameters.ignitionDelay / omega;
    event.combustionDurationSeconds = std::max(m_parameters.combustionDuration / omega, 1.0e-6);
}

double CompressionIgnitionModel::ignitionDelayTime(
    double temperature,
    double pressure,
    double equivalenceRatio)
{
    // D.N. Assanis, Z.S. Filipi, S.B. Fiveland, M. Syrimis, "A predictive
    // ignition delay correlation under steady-state and transient operation
    // of a direct injection diesel engine", J. Eng. Gas Turbines Power 125
    // (2003) 450-457. Fitted on instantaneous cylinder conditions for use in
    // the Livengood-Wu integral.
    const double pBar = std::max(pressure, 1.0e3) / 1.0e5;
    const double T = std::max(temperature, 200.0);
    const double phi = std::max(equivalenceRatio, 0.05);
    return 2.4e-3 * std::pow(phi, -0.2) * std::pow(pBar, -1.02) * std::exp(2100.0 / T);
}

CompressionIgnitionModel::StepResult CompressionIgnitionModel::stepHardware(
    Event &event,
    double dt,
    double cylinderTemperature,
    double cylinderPressure) const
{
    StepResult result;
    const Parameters &p = m_parameters;
    const double end = event.elapsed + dt;
    const double L = std::max(event.lengthScale, 1.0e-3);

    // Injection: nozzle hydraulics (Bernoulli through the holes).
    const double remaining = event.targetFuelMoles - event.injectedFuelMoles;
    if (remaining > 0.0 && event.fuelDensity > 0.0 && event.fuelMolecularMass > 0.0) {
        const double dp = std::max(0.0, p.injectionPressure - cylinderPressure);
        const double v = std::sqrt(2.0 * dp / event.fuelDensity);
        const double area = p.nozzleHoles * 0.25 * 3.14159265358979
            * p.nozzleHoleDiameter * p.nozzleHoleDiameter;
        const double massRate = p.nozzleDischargeCoefficient * area * event.fuelDensity * v;
        const double moles = std::min(remaining, massRate * dt / event.fuelMolecularMass);
        result.fuelMolesToInject = std::max(0.0, moles);
        event.injectedFuelMoles += result.fuelMolesToInject;
        event.sprayVelocity = v;
    }
    else {
        // After the end of injection the spray turbulence decays on the
        // turnover time L / u.
        const double u = p.sprayTurbulenceCoefficient * event.sprayVelocity + event.pistonTurbulence;
        event.sprayVelocity *= std::exp(-dt * u / L);
    }

    // Ignition (correlation or fixed delay), as on the prescribed path.
    if (p.ignitionDelayCorrelation && event.ignitionTime < 0.0 && event.injectedFuelMoles > 0.0) {
        event.ignitionIntegral += dt / ignitionDelayTime(
            cylinderTemperature, cylinderPressure, event.equivalenceRatio);
        if (event.ignitionIntegral >= 1.0) event.ignitionTime = end;
    }
    if (!p.ignitionDelayCorrelation && event.ignitionTime < 0.0 && end >= event.ignitionDelaySeconds) {
        event.ignitionTime = end;
    }
    const bool conditionsMet =
        cylinderTemperature >= p.autoignitionTemperature
        && cylinderPressure >= p.autoignitionPressure;

    // Mixing-controlled burn: fuel present and not yet burned, at the rate of
    // the turbulence through the chamber length scale. Fuel injected during
    // the delay is all available at ignition (premixed spike).
    if (event.ignitionTime >= 0.0 && conditionsMet) {
        event.combustionStarted = true;
        result.combustionStarted = true;
        const double u = event.pistonTurbulence + p.sprayTurbulenceCoefficient * event.sprayVelocity;
        const double available = std::max(0.0, event.injectedFuelMoles - event.demandedBurnFuelMoles);
        const double burn = available * (1.0 - std::exp(-dt * u / L));
        result.fuelMolesToBurn = burn;
        event.demandedBurnFuelMoles += burn;
    }

    event.elapsed = end;
    const bool injected = event.injectedFuelMoles >= event.targetFuelMoles * (1.0 - 1.0e-9);
    const bool burned = event.demandedBurnFuelMoles >= event.targetFuelMoles * 0.999;
    if ((injected && burned) || event.elapsed >= event.maxDuration) event.active = false;
    result.active = event.active;
    return result;
}

double CompressionIgnitionModel::wiebe(double normalizedProgress, double a, double m) {
    const double x = clamp01(normalizedProgress);
    return 1.0 - std::exp(-a * std::pow(x, m + 1.0));
}

double CompressionIgnitionModel::cumulativeBurnFraction(double normalizedProgress) const {
    const double x = std::max(0.0, normalizedProgress);
    const double premixed = m_parameters.premixedBurnFraction;
    const double premixedProgress = std::min(x / 0.35, 1.0);
    return clamp01(
        premixed * wiebe(premixedProgress, 5.0, 1.0)
        + (1.0 - premixed) * wiebe(x, 6.9, 2.0));
}

CompressionIgnitionModel::StepResult CompressionIgnitionModel::step(
    Event &event,
    double dt,
    double cylinderTemperature,
    double cylinderPressure) const
{
    StepResult result;
    if (!enabled() || !event.active || dt <= 0.0) return result;
    if (usesInjectionHardware()) return stepHardware(event, dt, cylinderTemperature, cylinderPressure);

    const double start = event.elapsed;
    const double end = start + dt;

    if (start < event.injectionDurationSeconds) {
        const double activeInjectionTime = std::max(
            0.0,
            std::min(end, event.injectionDurationSeconds) - start);
        const double inject = event.targetFuelMoles
            * activeInjectionTime / event.injectionDurationSeconds;
        result.fuelMolesToInject = std::max(0.0, inject);
        event.injectedFuelMoles += result.fuelMolesToInject;
    }

    const bool conditionsMet =
        cylinderTemperature >= m_parameters.autoignitionTemperature
        && cylinderPressure >= m_parameters.autoignitionPressure;

    // Ignition: fixed crank-angle delay, or the Livengood-Wu integral of the
    // ignition-delay correlation over the actual cylinder history from the
    // start of injection (fuel must be present).
    if (m_parameters.ignitionDelayCorrelation && event.ignitionTime < 0.0
        && event.injectedFuelMoles > 0.0)
    {
        event.ignitionIntegral += dt / ignitionDelayTime(
            cylinderTemperature, cylinderPressure, event.equivalenceRatio);
        if (event.ignitionIntegral >= 1.0) event.ignitionTime = end;
    }
    const double ignitionTime = m_parameters.ignitionDelayCorrelation
        ? event.ignitionTime
        : event.ignitionDelaySeconds;

    if (conditionsMet && ignitionTime >= 0.0 && end >= ignitionTime) {
        event.combustionStarted = true;
        result.combustionStarted = true;

        const double burnTime = std::max(0.0, end - ignitionTime);
        const double x = burnTime / event.combustionDurationSeconds;
        const double cumulativeDemand = event.targetFuelMoles * cumulativeBurnFraction(x);
        const double burnableDemand = std::min(cumulativeDemand, event.injectedFuelMoles);
        result.fuelMolesToBurn = std::max(
            0.0,
            burnableDemand - event.demandedBurnFuelMoles);
        event.demandedBurnFuelMoles += result.fuelMolesToBurn;
    }

    event.elapsed = end;
    // Correlation mode: an event that has not ignited by the end of the
    // nominal burn window (injection + combustion duration) is abandoned.
    const double ignitionReference = m_parameters.ignitionDelayCorrelation
        ? (event.ignitionTime >= 0.0 ? event.ignitionTime : event.injectionDurationSeconds)
        : event.ignitionDelaySeconds;
    const double endTime = std::max(
        event.injectionDurationSeconds,
        ignitionReference + event.combustionDurationSeconds);
    if (event.elapsed >= endTime) event.active = false;

    result.active = event.active;
    return result;
}
