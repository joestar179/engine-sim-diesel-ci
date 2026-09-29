#include "../include/fuel_rack_governor_model.h"

#include <algorithm>
#include <cmath>

namespace {
double clamp01(double x) { return std::max(0.0, std::min(1.0, x)); }
}

FuelRackGovernorModel::FuelRackGovernorModel() = default;

void FuelRackGovernorModel::initialize(const Parameters &parameters) {
    m_parameters = parameters;
    m_parameters.gamma = std::max(0.01, m_parameters.gamma);
    m_parameters.startingRack = clamp01(m_parameters.startingRack);
    m_parameters.k_p = std::max(0.0, m_parameters.k_p);
    m_parameters.crankRackLimit =
        std::max(m_parameters.startingRack, clamp01(m_parameters.crankRackLimit));
    reset();
    setSpeedControl(0.0);
}

void FuelRackGovernorModel::reset() {
    m_speedControl = 0.0;
    m_targetSpeed = m_parameters.minSpeed;
    m_rack = 0.0;
    m_rackRate = 0.0;
    m_output = 0.0;
    m_starting = true;
}

void FuelRackGovernorModel::setSpeedControl(double normalizedControl) {
    m_speedControl = clamp01(normalizedControl);
    m_targetSpeed = (1.0 - m_speedControl) * m_parameters.minSpeed
        + m_speedControl * m_parameters.maxSpeed;
}

double FuelRackGovernorModel::update(double dt, double engineSpeedRadPerSec) {
    if (dt <= 0.0) return std::pow(clamp01(m_output), m_parameters.gamma);

    const double speed = std::abs(engineSpeedRadPerSec);
    const double error = m_targetSpeed * m_targetSpeed - speed * speed;
    m_rackRate += dt * error * m_parameters.k_s - dt * m_rackRate * m_parameters.k_d;
    m_rackRate = std::max(m_parameters.minRackRate, std::min(m_parameters.maxRackRate, m_rackRate));
    m_rack += m_rackRate * dt;
    m_rack = clamp01(m_rack);

    // Proportional compensation acts on the linear speed error so its
    // effect does not depend on the operating speed squared.
    const double target = std::max(1.0, std::abs(m_targetSpeed));
    double output = clamp01(m_rack + m_parameters.k_p * (target - speed) / target);

    // startingRack and crankRackLimit are defined in commanded/output rack
    // space; convert them back through the gamma curve so the command reaching
    // Engine::setFuelRack() respects them.
    const double minSpeed = std::abs(m_parameters.minSpeed);
    if (speed < 0.5 * minSpeed) m_starting = true;
    else if (speed >= minSpeed) m_starting = false;

    if (m_starting) {
        // Start-fuel limit, also clamping the integral so it cannot wind up
        // while the engine sits at standstill with ignition on.
        const double rawCrankLimit =
            std::pow(m_parameters.crankRackLimit, 1.0 / m_parameters.gamma);
        m_rack = std::min(m_rack, rawCrankLimit);
        output = std::min(output, rawCrankLimit);
    }

    if (speed < 0.5 * minSpeed) {
        const double rawStartingRack =
            std::pow(m_parameters.startingRack, 1.0 / m_parameters.gamma);
        m_rack = std::max(m_rack, rawStartingRack);
        output = std::max(output, rawStartingRack);
    }

    m_output = output;
    return std::pow(output, m_parameters.gamma);
}
