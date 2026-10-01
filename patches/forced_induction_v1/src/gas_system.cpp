#include "../include/gas_system.h"

#include "../include/units.h"
#include "../include/utilities.h"

#include <algorithm>
#include <cmath>
#include <cassert>

void GasSystem::setGeometry(double width, double height, double dx, double dy) {
    m_width = width;
    m_height = height;
    m_dx = dx;
    m_dy = dy;
}

namespace {
// Harmonic-oscillator vibrational energy of air's diatomic molecules
// (Einstein function), mole fractions N2 0.79 / O2 0.21, characteristic
// vibrational temperatures 3353 K (N2) and 2239 K (O2). Tabulated at 1 K.
struct VibrationTable {
    static constexpr int Size = 8001; // 0 .. 8000 K
    double u[Size];
    double cv[Size];
    VibrationTable() {
        const double w[2] = { 0.79, 0.21 };
        const double theta[2] = { 3353.0, 2239.0 };
        for (int i = 0; i < Size; ++i) {
            const double T = static_cast<double>(i);
            double ui = 0.0, ci = 0.0;
            for (int k = 0; k < 2 && T > 0.0; ++k) {
                const double x = theta[k] / T;
                if (x > 700.0) continue;
                const double ex = std::exp(x);
                ui += w[k] * theta[k] / (ex - 1.0);
                ci += w[k] * x * x * ex / ((ex - 1.0) * (ex - 1.0));
            }
            u[i] = constants::R * ui;
            cv[i] = constants::R * ci;
        }
    }
};

// Combustion products CO2 : H2O = 1 : 1 (gasoline 47 : 53, diesel 51 : 49),
// harmonic oscillators per normal mode: CO2 bend 960 K (x2), symmetric
// stretch 1997 K, asymmetric stretch 3380 K; H2O bend 2295 K, stretches
// 5262 K and 5404 K (NIST fundamental frequencies). Vibrational part only;
// the rigid part (2.75 R) is added in GasSystem::kineticEnergyPerMol.
struct ProductVibrationTable {
    static constexpr int Size = 8001;
    double u[Size];
    double cv[Size];
    ProductVibrationTable() {
        const double w[7] = { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };
        const double theta[7] = { 960.0, 960.0, 1997.0, 3380.0, 2295.0, 5262.0, 5404.0 };
        for (int i = 0; i < Size; ++i) {
            const double T = static_cast<double>(i);
            double ui = 0.0, ci = 0.0;
            for (int k = 0; k < 7 && T > 0.0; ++k) {
                const double x = theta[k] / T;
                if (x > 700.0) continue;
                const double ex = std::exp(x);
                ui += w[k] * theta[k] / (ex - 1.0);
                ci += w[k] * x * x * ex / ((ex - 1.0) * (ex - 1.0));
            }
            u[i] = constants::R * ui;
            cv[i] = constants::R * ci;
        }
    }
};

// Fill the shared tables once at static initialisation.
const VibrationTable g_vibrationTable;
const ProductVibrationTable g_productVibrationTable;
struct VibrationTableExport {
    VibrationTableExport() {
        for (int i = 0; i < VibrationTable::Size; ++i) {
            gas_vibration::energy[i] = g_vibrationTable.u[i];
            gas_vibration::heatCapacity[i] = g_vibrationTable.cv[i];
            gas_vibration::productEnergy[i] = g_productVibrationTable.u[i];
            gas_vibration::productHeatCapacity[i] = g_productVibrationTable.cv[i];
        }
    }
} g_vibrationTableExport;
}

bool gas_vibration::enabled = true;
bool gas_vibration::enthalpyFlow = true;
double gas_vibration::energy[gas_vibration::TableSize];
double gas_vibration::heatCapacity[gas_vibration::TableSize];
bool gas_vibration::products = true;
double gas_vibration::productEnergy[gas_vibration::TableSize];
double gas_vibration::productHeatCapacity[gas_vibration::TableSize];

double GasSystem::pressureOf(double n, double E, double V, int degreesOfFreedom, double productFraction) {
    if (n <= 0.0 || V <= 0.0) return 0.0;
    return n * constants::R * temperatureFromEnergyPerMol(E / n, degreesOfFreedom, -1.0, productFraction) / V;
}

namespace {
// Refines a linear estimate of the equalising flow dn (secant method on the
// pressure difference). residual(dn) must be monotonic on [lo, hi].
template <typename F>
double solveEqualisingFlow(F residual, double estimate, double lo, double hi) {
    double x0 = std::min(hi, std::max(lo, estimate));
    double f0 = residual(x0);
    double x1 = std::min(hi, std::max(lo, x0 * 0.98));
    if (x1 == x0) x1 = std::min(hi, std::max(lo, x0 + 1.0e-9 * (hi - lo)));
    double f1 = residual(x1);
    for (int i = 0; i < 6; ++i) {
        if (f1 == f0) break;
        const double x2 = std::min(hi, std::max(lo, x1 - f1 * (x1 - x0) / (f1 - f0)));
        x0 = x1; f0 = f1;
        x1 = x2; f1 = residual(x1);
        if (std::abs(f1) < 1.0e-9) break;
    }
    return x1;
}
}

void GasSystem::initialize(double P, double V, double T, const Mix &mix, int degreesOfFreedom) {
    m_degreesOfFreedom = degreesOfFreedom;
    m_state.n_mol = P * V / (constants::R * T);
    m_state.V = V;
    m_state.E_k = m_state.n_mol * kineticEnergyPerMol(T, degreesOfFreedom, mix.p_products);
    m_state.mix = mix;
    m_state.momentum[0] = m_state.momentum[1] = 0;

    const double hcr = heatCapacityRatio();
    m_chokedFlowLimit = chokedFlowLimit(degreesOfFreedom);
    m_chokedFlowFactorCached = chokedFlowRate(degreesOfFreedom);
}

void GasSystem::reset(double P, double T, const Mix &mix) {
    m_state.n_mol = P * volume() / (constants::R * T);
    m_state.E_k = m_state.n_mol * kineticEnergyPerMol(T, m_degreesOfFreedom, mix.p_products);
    m_state.mix = mix;
    m_state.momentum[0] = m_state.momentum[1] = 0;
}

void GasSystem::setVolume(double V) {
    return changeVolume(V - m_state.V);
}

void GasSystem::setN(double n) {
    m_state.E_k = kineticEnergy(n);
    m_state.n_mol = n;
}

void GasSystem::changeVolume(double dV) {
    const double V = this->volume();
    const double L = std::pow(V + dV, 1 / 3.0);
    const double surfaceArea = (L * L);
    const double dL = -dV / surfaceArea;
    const double W = dL * pressure() * surfaceArea;

    m_state.V += dV;
    m_state.E_k += W;
}

void GasSystem::changePressure(double dP) {
    // At constant n and V: dT = dP V / (n R), dE = n u(T + dT) - n u(T).
    if (n() <= 0) {
        m_state.E_k += dP * volume() * m_degreesOfFreedom * 0.5;
        return;
    }
    changeTemperature(dP * volume() / (n() * constants::R));
}

void GasSystem::changeTemperature(double dT) {
    if (n() <= 0) return;
    const double T = temperature();
    m_state.E_k = n() * kineticEnergyPerMol(std::max(0.0, T + dT), m_degreesOfFreedom, m_state.mix.p_products);
}

void GasSystem::changeEnergy(double dE) {
    m_state.E_k += dE;
}

void GasSystem::changeMix(const Mix &mix) {
    m_state.mix = mix;
}

void GasSystem::injectFuel(double n) {
    if (n <= 0.0) return;
    Mix fuel;
    fuel.p_fuel = 1.0;
    fuel.p_inert = 0.0;
    fuel.p_o2 = 0.0;
    // Represent vaporized direct-injected fuel as part of the chamber gas.
    // Sensible energy is matched to the receiving gas; chemical energy is
    // released separately by the combustion model.
    gainN(n, kineticEnergyPerMol(), fuel);
}

double GasSystem::reactFuel(double fuelMoles, double idealO2PerFuel) {
    if (fuelMoles <= 0.0 || idealO2PerFuel <= 0.0 || n() <= 0.0) return 0.0;

    const double availableFuel = n_fuel();
    const double availableO2 = n_o2();
    const double reactedFuel = std::fmin(
        std::fmin(fuelMoles, availableFuel),
        availableO2 / idealO2PerFuel);
    if (reactedFuel <= 0.0) return 0.0;

    const double reactedO2 = reactedFuel * idealO2PerFuel;
    const double total = n();
    const double inertProducts = reactedFuel + reactedO2;
    m_state.mix.p_fuel = (availableFuel - reactedFuel) / total;
    m_state.mix.p_o2 = (availableO2 - reactedO2) / total;
    m_state.mix.p_products = (m_state.mix.p_products * total + inertProducts) / total;
    m_state.mix.p_inert = (n_inert() + inertProducts) / total;
    return reactedFuel;
}

void GasSystem::changeTemperature(double dT, double n) {
    const double T = temperature();
    m_state.E_k += n * (kineticEnergyPerMol(std::max(0.0, T + dT), m_degreesOfFreedom, m_state.mix.p_products)
        - kineticEnergyPerMol(T, m_degreesOfFreedom, m_state.mix.p_products));
}

double GasSystem::react(double n, const Mix &mix) {
    const double l_n_fuel = mix.p_fuel * n;
    const double l_n_o2 = mix.p_o2 * n;

    const double system_n_fuel = n_fuel();
    const double system_n_o2 = n_o2();
    const double system_n_inert = n_inert();
    const double system_n = this->n();

    // Assuming the following reaction:
    // 25[O2] + 2[C8H16] -> 16[CO2] + 18[H2O]
    constexpr double ideal_o2_ratio = 25.0 / 2;
    constexpr double ideal_fuel_ratio = 2.0 / 25;
    constexpr double output_input_ratio = (16.0 + 18.0) / (25 + 2);

    const double ideal_fuel_n = ideal_fuel_ratio * l_n_o2;
    const double ideal_o2_n = ideal_o2_ratio * l_n_fuel;
    
    const double a_n_fuel = std::fmin(
        std::fmin(system_n_fuel, l_n_fuel),
        ideal_fuel_n);
    const double a_n_o2 = std::fmin(
        std::fmin(system_n_o2, l_n_o2),
        ideal_o2_n);

    const double reactants_n = a_n_fuel + a_n_o2;
    const double products_n = output_input_ratio * reactants_n;
    const double dn = products_n - reactants_n;

    m_state.n_mol += dn;

    // Adjust mix
    const double new_system_n_fuel = system_n_fuel - a_n_fuel;
    const double new_system_n_o2 = system_n_o2 - a_n_o2;
    const double new_system_n_inert = system_n_inert + products_n;
    const double new_system_n = system_n + dn;
    const double new_system_n_products = m_state.mix.p_products * system_n + products_n;

    if (new_system_n != 0) {
        m_state.mix.p_fuel = new_system_n_fuel / new_system_n;
        m_state.mix.p_inert = new_system_n_inert / new_system_n;
        m_state.mix.p_o2 = new_system_n_o2 / new_system_n;
        m_state.mix.p_products = new_system_n_products / new_system_n;
    }
    else {
        m_state.mix.p_fuel = m_state.mix.p_inert = m_state.mix.p_o2 = 0;
    }

    return a_n_fuel;
}

double GasSystem::flowConstant(
    double targetFlowRate,
    double P,
    double pressureDrop,
    double T,
    double hcr)
{
    const double T_0 = T;
    const double p_0 = P, p_T = P - pressureDrop; // p_0 = upstream pressure

    const double chokedFlowLimit =
        std::pow((2.0 / (hcr + 1)), hcr / (hcr - 1));
    const double p_ratio = p_T / p_0;

    double flowRate = 0;
    if (p_ratio <= chokedFlowLimit) {
        // Choked flow
        flowRate = std::sqrt(hcr);
        flowRate *= std::pow(2 / (hcr + 1), (hcr + 1) / (2 * (hcr - 1)));
    }
    else {
        flowRate = (2 * hcr) / (hcr - 1);
        flowRate *= (1 - std::pow(p_ratio, (hcr - 1) / hcr));
        flowRate = std::sqrt(flowRate);
        flowRate *= std::pow(p_ratio, 1 / hcr);
    }

    flowRate *= p_0 / std::sqrt(constants::R * T_0);

    return targetFlowRate / flowRate;
}

double GasSystem::k_28inH2O(double flowRateScfm) {
    return flowConstant(
        units::flow(flowRateScfm, units::scfm),
        units::pressure(1.0, units::atm),
        units::pressure(28.0, units::inH2O),
        units::celcius(25),
        heatCapacityRatio(5)
    );
}

double GasSystem::k_carb(double flowRateScfm) {
    return flowConstant(
        units::flow(flowRateScfm, units::scfm),
        units::pressure(1.0, units::atm),
        units::pressure(1.5, units::inHg),
        units::celcius(25),
        heatCapacityRatio(5)
    );
}

double GasSystem::flowRate(
    double k_flow,
    double P0,
    double P1,
    double T0,
    double T1,
    double hcr,
    double chokedFlowLimit,
    double chokedFlowRateCached)
{
    if (k_flow == 0) return 0;

    double direction;
    double T_0;
    double p_0, p_T; // p_0 = upstream pressure
    if (P0 > P1) {
        direction = 1.0;
        T_0 = T0;
        p_0 = P0;
        p_T = P1;
    }
    else {
        direction = -1.0;
        T_0 = T1;
        p_0 = P1;
        p_T = P0;
    }

    const double p_ratio = p_T / p_0;
    double flowRate = 0;
    if (p_ratio <= chokedFlowLimit) {
        // Choked flow
        flowRate = chokedFlowRateCached;
        flowRate /= std::sqrt(constants::R * T_0);
    }
    else {
        const double s = std::pow(p_ratio, 1 / hcr);

        flowRate = (2 * hcr) / (hcr - 1);
        flowRate *= s * (s - p_ratio);
        flowRate = std::sqrt(std::fmax(flowRate, 0.0) / (constants::R * T_0));
    }

    flowRate *= direction * p_0;

    return flowRate * k_flow;
}

double GasSystem::loseN(double dn, double E_k_per_mol) {
    m_state.E_k -= E_k_per_mol * dn;
    m_state.n_mol -= dn;

    if (m_state.n_mol < 0) {
        m_state.n_mol = 0;
    }

    return dn;
}

double GasSystem::gainN(double dn, double E_k_per_mol, const Mix &mix) {
    const double next_n = m_state.n_mol + dn;
    const double current_n = m_state.n_mol;

    m_state.E_k += dn * E_k_per_mol;
    m_state.n_mol = next_n;

    if (next_n != 0) {
        m_state.mix.p_fuel = (m_state.mix.p_fuel * current_n + dn * mix.p_fuel) / next_n;
        m_state.mix.p_inert = (m_state.mix.p_inert * current_n + dn * mix.p_inert) / next_n;
        m_state.mix.p_o2 = (m_state.mix.p_o2 * current_n + dn * mix.p_o2) / next_n;
        m_state.mix.p_products = std::max(0.0, (m_state.mix.p_products * current_n + dn * mix.p_products) / next_n);
    }
    else {
        m_state.mix.p_fuel = m_state.mix.p_inert = m_state.mix.p_o2 = 0;
    }

    return -dn;
}

void GasSystem::dissipateExcessVelocity() {
    const double v_x = velocity_x();
    const double v_y = velocity_y();
    const double v_squared = v_x * v_x + v_y * v_y;
    const double c = this->c();
    const double c_squared = c * c;

    if (c_squared >= v_squared || v_squared == 0) {
        return;
    }

    const double k_squared = c_squared / v_squared;
    const double k = std::sqrt(k_squared);

    m_state.momentum[0] *= k;
    m_state.momentum[1] *= k;

    m_state.E_k += 0.5 * mass() * (v_squared - c_squared);

    if (m_state.E_k < 0) m_state.E_k = 0;
}

void GasSystem::updateVelocity(double dt, double beta) {
    if (n() == 0) return;

    const double depth = volume() / (m_width * m_height);
    
    double d_momentum_x = 0;
    double d_momentum_y = 0;

    const double p0 = dynamicPressure(m_dx, m_dy);
    const double p1 = dynamicPressure(-m_dx, -m_dy);
    const double p2 = dynamicPressure(m_dy, m_dx);
    const double p3 = dynamicPressure(-m_dy, -m_dx);

    const double p_sa_0 = p0 * (m_height * depth);
    const double p_sa_1 = p1 * (m_height * depth);
    const double p_sa_2 = p2 * (m_width * depth);
    const double p_sa_3 = p3 * (m_width * depth);

    d_momentum_x += p_sa_0 * m_dx;
    d_momentum_y += p_sa_0 * m_dy;

    d_momentum_x -= p_sa_1 * m_dx;
    d_momentum_y -= p_sa_1 * m_dy;

    d_momentum_x += p_sa_2 * m_dy;
    d_momentum_y += p_sa_2 * m_dx;

    d_momentum_x -= p_sa_3 * m_dy;
    d_momentum_y -= p_sa_3 * m_dx;

    const double m = mass();
    const double inv_m = 1 / m;
    const double v0_x = m_state.momentum[0] * inv_m;
    const double v0_y = m_state.momentum[1] * inv_m;

    m_state.momentum[0] -= d_momentum_x * dt * beta;
    m_state.momentum[1] -= d_momentum_y * dt * beta;

    const double v1_x = m_state.momentum[0] * inv_m;
    const double v1_y = m_state.momentum[1] * inv_m;

    m_state.E_k -= 0.5 * m * (v1_x * v1_x - v0_x * v0_x);
    m_state.E_k -= 0.5 * m * (v1_y * v1_y - v0_y * v0_y);

    if (m_state.E_k < 0) m_state.E_k = 0;
}

void GasSystem::dissipateVelocity(double dt, double timeConstant) {
    if (n() == 0) return;

    const double invMass = 1.0 / mass();
    const double velocity_x = m_state.momentum[0] * invMass;
    const double velocity_y = m_state.momentum[1] * invMass;
    const double velocity_squared =
        velocity_x * velocity_x + velocity_y * velocity_y;

    const double s = dt / (dt + timeConstant);
    m_state.momentum[0] = m_state.momentum[0] * (1 - s);
    m_state.momentum[1] = m_state.momentum[1] * (1 - s);

    const double newVelocity_x = m_state.momentum[0] * invMass;
    const double newVelocity_y = m_state.momentum[1] * invMass;
    const double newVelocity_squared =
        newVelocity_x * newVelocity_x + newVelocity_y * newVelocity_y;

    const double dE_k = 0.5 * mass() * (velocity_squared - newVelocity_squared);
    m_state.E_k += dE_k;
}

double GasSystem::flow(const FlowParameters &params) {
    GasSystem *source = nullptr, *sink = nullptr;
    double sourcePressure = 0, sinkPressure = 0;
    double dx, dy;
    double sourceCrossSection = 0, sinkCrossSection = 0;
    double direction = 0;

    const double P_0 =
        params.system_0->pressure()
        + params.system_0->dynamicPressure(params.direction_x, params.direction_y);
    const double P_1 =
        params.system_1->pressure()
        + params.system_1->dynamicPressure(-params.direction_x, -params.direction_y);

    if (P_0 > P_1) {
        dx = params.direction_x;
        dy = params.direction_y;
        source = params.system_0;
        sink = params.system_1;
        sourcePressure = P_0;
        sinkPressure = P_1;
        sourceCrossSection = params.crossSectionArea_0;
        sinkCrossSection = params.crossSectionArea_1;
        direction = 1.0;
    }
    else {
        dx = -params.direction_x;
        dy = -params.direction_y;
        source = params.system_1;
        sink = params.system_0;
        sourcePressure = P_1;
        sinkPressure = P_0;
        sourceCrossSection = params.crossSectionArea_1;
        sinkCrossSection = params.crossSectionArea_0;
        direction = -1.0;
    }

    double flow = params.dt * flowRate(
        params.k_flow,
        sourcePressure,
        sinkPressure,
        source->temperature(),
        sink->temperature(),
        source->heatCapacityRatio(),
        source->m_chokedFlowLimit,
        source->m_chokedFlowFactorCached);

    // (The upstream code computed pressureEquilibriumMaxFlow(sink) here but
    // never used it; the call is dropped because it is now an iterative solve.)
    flow = clamp(flow, 0.0, 0.9 * source->n());

    const double fraction = flow / source->n();
    const double fractionVolume = fraction * source->volume();
    const double fractionMass = fraction * source->mass();
    const double remainingMass = (1 - fraction) * source->mass();

    if (flow != 0) {
        // - Stage 1
        // Fraction flows from source to sink.

        const double E_k_bulk_src0 = source->bulkKineticEnergy();
        const double E_k_bulk_sink0 = sink->bulkKineticEnergy();

        const double s0 = source->totalEnergy() + sink->totalEnergy();

        // Gas crossing a control-volume boundary carries its enthalpy
        // (internal energy + flow work p*v = (dof/2 + 1) R T per mole), not
        // only its internal energy. Transferring internal energy alone left
        // a filling cylinder too cold and too dense (the 16-251B trapped
        // ~1.2-1.3x the ambient-density charge without any boost).
        const double E_k_per_mol = source->enthalpyPerMol();
        sink->gainN(flow, E_k_per_mol, source->mix());
        source->loseN(flow, E_k_per_mol);

        const double s1 = source->totalEnergy() + sink->totalEnergy();

        const double dp_x = source->m_state.momentum[0] * fraction;
        const double dp_y = source->m_state.momentum[1] * fraction;
        source->m_state.momentum[0] -= dp_x;
        source->m_state.momentum[1] -= dp_y;

        sink->m_state.momentum[0] += dp_x;
        sink->m_state.momentum[1] += dp_y;

        const double E_k_bulk_src1 = source->bulkKineticEnergy();
        const double E_k_bulk_sink1 = sink->bulkKineticEnergy();

        sink->m_state.E_k -= ((E_k_bulk_src1 + E_k_bulk_sink1) - (E_k_bulk_src0 + E_k_bulk_sink0));
    }
    
    const double sourceMass = source->mass();
    const double invSourceMass = 1 / sourceMass;
    const double sinkMass = sink->mass();
    const double invSinkMass = 1 / sinkMass;

    const double c_source = source->c();
    const double c_sink = sink->c();

    const double sourceInitialMomentum_x = source->m_state.momentum[0];
    const double sourceInitialMomentum_y = source->m_state.momentum[1];

    const double sinkInitialMomentum_x = sink->m_state.momentum[0];
    const double sinkInitialMomentum_y = sink->m_state.momentum[1];

    // Momentum in fraction

    if (sinkCrossSection != 0) {
        const double sinkFractionVelocity =
            clamp((fractionVolume / sinkCrossSection) / params.dt, 0.0, c_sink);
        const double sinkFractionVelocity_squared = sinkFractionVelocity * sinkFractionVelocity;
        const double sinkFractionVelocity_x = sinkFractionVelocity * dx;
        const double sinkFractionVelocity_y = sinkFractionVelocity * dy;
        const double sinkFractionMomentum_x = sinkFractionVelocity_x * fractionMass;
        const double sinkFractionMomentum_y = sinkFractionVelocity_y * fractionMass;

        sink->m_state.momentum[0] += sinkFractionMomentum_x;
        sink->m_state.momentum[1] += sinkFractionMomentum_y;
    }

    if (sourceCrossSection != 0 && sourceMass != 0) {
        const double sourceFractionVelocity =
            clamp((fractionVolume / sourceCrossSection) / params.dt, 0.0, c_source);
        const double sourceFractionVelocity_squared = sourceFractionVelocity * sourceFractionVelocity;
        const double sourceFractionVelocity_x = sourceFractionVelocity * dx;
        const double sourceFractionVelocity_y = sourceFractionVelocity * dy;
        const double sourceFractionMomentum_x = sourceFractionVelocity_x * fractionMass;
        const double sourceFractionMomentum_y = sourceFractionVelocity_y * fractionMass;

        source->m_state.momentum[0] += sourceFractionMomentum_x;
        source->m_state.momentum[1] += sourceFractionMomentum_y;
    }

    if (sourceMass != 0) {
        // Energy conservation
        const double sourceVelocity0_x = sourceInitialMomentum_x * invSourceMass;
        const double sourceVelocity0_y = sourceInitialMomentum_y * invSourceMass;

        const double sourceVelocity1_x = source->m_state.momentum[0] * invSourceMass;
        const double sourceVelocity1_y = source->m_state.momentum[1] * invSourceMass;

        source->m_state.E_k -=
            0.5 * sourceMass
            * (sourceVelocity1_x * sourceVelocity1_x - sourceVelocity0_x * sourceVelocity0_x);

        source->m_state.E_k -=
            0.5 * sourceMass
            * (sourceVelocity1_y * sourceVelocity1_y - sourceVelocity0_y * sourceVelocity0_y);
    }

    if (sinkMass > 0) {
        const double sinkVelocity0_x = sinkInitialMomentum_x * invSinkMass;
        const double sinkVelocity0_y = sinkInitialMomentum_y * invSinkMass;

        const double sinkVelocity1_x = sink->m_state.momentum[0] * invSinkMass;
        const double sinkVelocity1_y = sink->m_state.momentum[1] * invSinkMass;

        sink->m_state.E_k -=
            0.5 * sinkMass
            * (sinkVelocity1_x * sinkVelocity1_x - sinkVelocity0_x * sinkVelocity0_x);

        sink->m_state.E_k -=
            0.5 * sinkMass
            * (sinkVelocity1_y * sinkVelocity1_y - sinkVelocity0_y * sinkVelocity0_y);
    }

    if (sink->m_state.E_k < 0) {
        sink->m_state.E_k = 0;
    }

    if (source->m_state.E_k < 0) {
        source->m_state.E_k = 0;
    }

    return flow * direction;
}

double GasSystem::flow(double k_flow, double dt, double P_env, double T_env, const Mix &mix) {
    double flow = dt * flowRate(
        k_flow,
        pressure(),
        P_env,
        temperature(),
        T_env,
        heatCapacityRatio(),
        m_chokedFlowLimit,
        m_chokedFlowFactorCached);

    // The exact equalising limit is only needed when the requested flow is
    // near it; the linear estimate (exact for constant cv) screens cheaply.
    const double f = effectiveDegreesOfFreedom();
    const double linearMax = -(P_env * (0.5 * f * volume()) - kineticEnergy())
        / ((pressure() > P_env) ? enthalpyPerMol() : enthalpyPerMol(T_env, m_degreesOfFreedom, mix.p_products));
    const double maxFlow = (std::abs(flow) > 0.5 * std::abs(linearMax))
        ? pressureEquilibriumMaxFlow(P_env, T_env)
        : linearMax;
    if (std::abs(flow) > std::abs(maxFlow)) {
        flow = maxFlow;
    }

    if (flow < 0) {
        const double bulk_E_k_0 = bulkKineticEnergy();
        // Inflow from the environment brings its enthalpy (see flow()).
        gainN(-flow,
            enthalpyPerMol(T_env, m_degreesOfFreedom, mix.p_products),
            mix);
        const double bulk_E_k_1 = bulkKineticEnergy();

        m_state.E_k += (bulk_E_k_1 - bulk_E_k_0);
    }
    else {
        const double starting_n = n();
        loseN(flow, enthalpyPerMol());

        m_state.momentum[0] -= (flow / starting_n) * m_state.momentum[0];
        m_state.momentum[1] -= (flow / starting_n) * m_state.momentum[1];
    }

    return flow;
}

double GasSystem::pressureEquilibriumMaxFlow(const GasSystem *b) const {
    // pressure_a = (kineticEnergy() + n * b->kineticEnergyPerMol()) / (0.5 * degreesOfFreedom * volume())
    // pressure_b = (b->kineticEnergy() - n *  / (0.5 * b->degreesOfFreedom * b->volume())
    // pressure_a = pressure_b

    // E_a = kineticEnergy()
    // E_b = b->kineticEnergy()
    // D_a = E_a / n()
    // D_b = E_b / b->n()
    // Q_a = 1 / (0.5 * degreesOfFreedom * volume())
    // Q_b = 1 / (0.5 * b->degreesOfFreedom * b->volume())
    // pressure_a = Q_a * (E_a + dn * D_b)
    // pressure_b = Q_b * (E_b - dn * D_b)

    // Transferred energy per mole is the source's enthalpy (see flow()).
    if (pressure() > b->pressure()) {
        // Equal final pressures with P = 2E/(fV) and each side's secant f:
        // dn = (f_b V_b E_a - f_a V_a E_b) / (h (f_b V_b + f_a V_a)).
        const double h = enthalpyPerMol();
        const double fa = effectiveDegreesOfFreedom(), fb = b->effectiveDegreesOfFreedom();
        const double estimate =
                (fb * b->volume() * kineticEnergy() - fa * volume() * b->kineticEnergy()) /
                (h * (fb * b->volume() + fa * volume()));
        // Exact equalisation: cv depends on temperature, so refine the
        // linear estimate on the real pressures.
        const double na = n(), Ea = kineticEnergy(), Va = volume();
        const double nb = b->n(), Eb = b->kineticEnergy(), Vb = b->volume();
        const int da = m_degreesOfFreedom, db = b->m_degreesOfFreedom;
        const double xa = m_state.mix.p_products, xb = b->m_state.mix.p_products;
        const double maxFlow = solveEqualisingFlow([&](double dn) {
            return pressureOf(na - dn, Ea - dn * h, Va, da, xa) - pressureOf(nb + dn, Eb + dn * h, Vb, db, xb);
        }, estimate, 0.0, na);
        return std::fmax(0.0, std::fmin(maxFlow, n()));
    }
    else {
        const double h = b->enthalpyPerMol();
        const double fa = effectiveDegreesOfFreedom(), fb = b->effectiveDegreesOfFreedom();
        const double estimate =
                (fb * b->volume() * kineticEnergy() - fa * volume() * b->kineticEnergy()) /
                (h * (fb * b->volume() + fa * volume()));
        const double na = n(), Ea = kineticEnergy(), Va = volume();
        const double nb = b->n(), Eb = b->kineticEnergy(), Vb = b->volume();
        const int da = m_degreesOfFreedom, db = b->m_degreesOfFreedom;
        const double xa = m_state.mix.p_products, xb = b->m_state.mix.p_products;
        const double moved = solveEqualisingFlow([&](double m) {
            return pressureOf(nb - m, Eb - m * h, Vb, db, xb) - pressureOf(na + m, Ea + m * h, Va, da, xa);
        }, -estimate, 0.0, nb);
        return std::fmin(0.0, std::fmax(-moved, -b->n()));
    }
}

double GasSystem::pressureEquilibriumMaxFlow(double P_env, double T_env) const {
    if (pressure() > P_env) {
        // Linearised with the current secant degrees of freedom (P = 2E/(fV)).
        const double f = effectiveDegreesOfFreedom();
        const double h = enthalpyPerMol();
        const double estimate = -(P_env * (0.5 * f * volume()) - kineticEnergy()) / h;
        const double n0 = n(), E0 = kineticEnergy(), V0 = volume();
        const int d = m_degreesOfFreedom;
        return solveEqualisingFlow([&](double dn) {
            return pressureOf(n0 - dn, E0 - dn * h, V0, d, m_state.mix.p_products) - P_env;
        }, estimate, 0.0, n0);
    }
    else {
        const double f = effectiveDegreesOfFreedom();
        const double h = enthalpyPerMol(T_env, m_degreesOfFreedom);
        const double estimate = -(P_env * (0.5 * f * volume()) - kineticEnergy()) / h;
        const double n0 = n(), E0 = kineticEnergy(), V0 = volume();
        const int d = m_degreesOfFreedom;
        const double gained = solveEqualisingFlow([&](double m) {
            return P_env - pressureOf(n0 + m, E0 + m * h, V0, d, m_state.mix.p_products);
        }, -estimate, 0.0, std::max(1.0e-12, -4.0 * estimate));
        return -gained;
    }
}
