#include "../include/engine_friction_model.h"

#include <algorithm>
#include <cmath>

EngineFrictionModel::Breakdown EngineFrictionModel::fmep(double rpm, double intakeOverAmbient) const {
    Breakdown f;
    if (!enabled()) return f;

    const Parameters &p = m_parameters;
    // The (1 + 1000/N) boundary-friction factors diverge at standstill; the
    // model is used from 100 rpm (cranking) upward.
    const double N = std::max(100.0, std::abs(rpm));
    const double B = p.bore, S = p.stroke, nc = p.cylinders;
    const double Up = 2.0 * (S / 1000.0) * N / 60.0;
    const double visc = p.viscosityRatio;
    constexpr double K = 2.38e-2;   // s/m

    const double Dm = p.mainBearingDiameter, Lm = p.mainBearingLength, nm = p.mainBearings;
    f.crankshaft =
        1.22e5 * Dm / (B * B * S * nc)
        + visc * 3.03e-4 * N * Dm * Dm * Dm * Lm * nm / (B * B * S * nc)
        + 1.35e-10 * Dm * Dm * N * N * nm / nc;

    const double Dr = p.rodBearingDiameter, Lr = p.rodBearingLength;
    f.reciprocating =
        visc * 2.94e2 * Up / B
        + 4.06e4 * (1.0 + 1000.0 / N) / (B * B)
        + visc * 3.03e-4 * N * Dr * Dr * Dr * Lr * nc / (B * B * S * nc);

    const double rc = p.compressionRatio;
    f.gasLoading = 6.89 * std::max(0.0, intakeOverAmbient)
        * (0.088 * rc + 0.182 * std::pow(rc, 1.33 - K * Up));

    const double nv = p.valves, Lv = p.maxValveLift, nb = p.camBearings;
    f.valvetrain =
        visc * 244.0 * N * nb / (B * B * S * nc) + 4.12
        + p.flatFollower * (1.0 + 1000.0 / N) * nv / (S * nc)
        + p.rollerFollower * N * nv / (S * nc)
        + visc * p.oscillatingHydrodynamic * std::pow(Lv, 1.5) * std::sqrt(N) * nv / (B * S * nc)
        + p.oscillatingMixed * (1.0 + 1000.0 / N) * Lv * nv / (S * nc);

    f.auxiliary = 6.23 + 5.22e-3 * N - 1.79e-7 * N * N;

    f.total = f.crankshaft + f.reciprocating + f.gasLoading + f.valvetrain + f.auxiliary;
    return f;
}

double EngineFrictionModel::torque(double rpm, double intakeOverAmbient) const {
    if (!enabled()) return 0.0;
    const Parameters &p = m_parameters;
    // fmep (kPa) x total displacement / (4 pi) for a four-stroke engine.
    const double displacement = p.cylinders * 3.14159265358979 / 4.0
        * (p.bore / 1000.0) * (p.bore / 1000.0) * (p.stroke / 1000.0);
    return fmep(rpm, intakeOverAmbient).total * 1.0e3 * displacement / (4.0 * 3.14159265358979);
}
