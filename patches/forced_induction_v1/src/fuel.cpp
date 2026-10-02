#include "../include/fuel.h"

#include "../include/units.h"

#include <cmath>

Fuel::Fuel() {
    m_molecularMass = 0.0;
    m_energyDensity = 0.0;
    m_density = 0.0;
    m_turbulenceToFlameSpeedRatio = nullptr;
    m_molecularAfr = 0.0;
    m_maxBurningEfficiency = 0.0;
    m_maxDilutionEffect = 0.0;
    m_maxTurbulenceEffect = 0.0;
    m_burningEfficiencyRandomness = 0.0;
    m_lowEfficiencyAttenuation = 0.0;
}

Fuel::~Fuel() {
    /* void */
}

void Fuel::initialize(const Parameters &params) {
    m_molecularMass = params.molecularMass;
    m_energyDensity = params.energyDensity;
    m_density = params.density;
    m_turbulenceToFlameSpeedRatio = params.turbulenceToFlameSpeedRatio;
    m_molecularAfr = params.molecularAfr;
    m_burningEfficiencyRandomness = params.burningEfficiencyRandomness;
    m_maxBurningEfficiency = params.maxBurningEfficiency;
    m_maxDilutionEffect = params.maxDilutionEffect;
    m_maxTurbulenceEffect = params.maxTurbulenceEffect;
    m_lowEfficiencyAttenuation = params.lowEfficiencyAttenuation;
    m_laminarPeakMixture = params.laminarPeakMixture;
    m_laminarPeakSpeed = params.laminarPeakSpeed;
    m_laminarSpeedCurvature = params.laminarSpeedCurvature;
    m_laminarAlpha0 = params.laminarAlpha0;
    m_laminarAlpha1 = params.laminarAlpha1;
    m_laminarAlpha2 = params.laminarAlpha2;
    m_laminarBeta0 = params.laminarBeta0;
    m_laminarBeta1 = params.laminarBeta1;
    m_laminarBeta2 = params.laminarBeta2;
}

double Fuel::flameSpeed(
    double turbulence,
    double molecularAfr,
    double T,
    double P,
    double firingPressure,
    double motoringPressure) const
{
    const double S_L = laminarBurningVelocity(molecularAfr, T, P);
    const double p_adjustment = 1.0;

    return m_turbulenceToFlameSpeedRatio->sampleTriangle((turbulence / S_L) * p_adjustment) * S_L;
}

double Fuel::laminarBurningVelocity(double molecularAfr, double T, double P) const {
    // Fuel-specific coefficients (fuel inputs; defaults = stock gasoline).
    const double er_m = m_laminarPeakMixture;
    const double B_m = m_laminarPeakSpeed;
    const double B_er = m_laminarSpeedCurvature;
    const double er = molecularAfr / m_molecularAfr;
    const double alpha = m_laminarAlpha0 + m_laminarAlpha1 * std::pow(er, m_laminarAlpha2);
    const double beta = m_laminarBeta0 + m_laminarBeta1 * std::pow(er, m_laminarBeta2);

    const double S_L_0 = B_m + B_er * (er - er_m) * (er - er_m);
    const double T_ratio = T / units::kelvin(298);
    const double P_ratio = P / units::pressure(1.0, units::atm);

    return S_L_0 * std::pow(T_ratio, alpha) * std::pow(P_ratio, beta);
}
