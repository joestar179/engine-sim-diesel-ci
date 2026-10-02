"""Effect of burned-gas dissociation on the fuel-air Otto cycle (Cantera).

Ideal constant-volume fuel-air cycle at the GX390 compression ratio:
isentropic compression of air + iso-octane vapour (frozen), combustion at
TDC at constant volume, isentropic expansion.

  equilibrium : burned gas at chemical equilibrium (UV), expansion in shifting
                equilibrium (SV) -> the real fuel-air cycle with dissociation.
  frozen      : burned gas = major products only (CO2, H2O, N2, O2; rich: CO
                and H2 in water-gas equilibrium at 1700 K, K = 3.5) at the same
                internal energy, expanded frozen -> no dissociation (the
                simulator's present gas model).

Work per kg of air, so the lambda trend is the power trend at fixed airflow.
Uses nasa_gas.yaml (NASA Glenn thermodynamic data shipped with Cantera).

usage: python dissociation_cycle.py
"""
import cantera as ct

CR = 8.2
T1, P1 = 330.0, 1.0e5
FUEL = 'C8H18,isooctane'
SPECIES = ['C8H18,isooctane', 'O2', 'N2', 'CO2', 'H2O', 'CO', 'H2', 'OH', 'H', 'O', 'NO', 'N']
_all = {sp.name: sp for sp in ct.Species.list_from_file('nasa_gas.yaml')}
gas = ct.Solution(thermo='ideal-gas', species=[_all[s] for s in SPECIES])

def unburned(lam):
    # C8H18 + 12.5 O2 -> 8 CO2 + 9 H2O
    o2 = 12.5 * lam
    return {FUEL: 1.0, 'O2': o2, 'N2': o2 * 3.76}, o2 * (32.0 + 3.76 * 28.013)

def frozen_products(lam):
    o2 = 12.5 * lam
    C, H = 8.0, 18.0
    if lam >= 1.0:
        return {'CO2': C, 'H2O': H / 2, 'O2': o2 - 12.5, 'N2': o2 * 3.76}
    K = 3.5   # [CO][H2O]/([CO2][H2])
    lo, hi = 0.0, C
    for _ in range(80):
        a = 0.5 * (lo + hi)                     # CO2
        h2o = K * a * (H / 2) / ((C - a) + K * a)
        if 2 * a + (C - a) + h2o < 2 * o2: lo = a
        else: hi = a
    h2o = K * a * (H / 2) / ((C - a) + K * a)
    return {'CO2': a, 'CO': C - a, 'H2O': h2o, 'H2': H / 2 - h2o, 'N2': o2 * 3.76}

def run(lam, equilibrium):
    mix, m_air = unburned(lam)
    gas.TPX = T1, P1, mix
    m_mix = gas.mean_molecular_weight * sum(mix.values())   # g per "fuel molecule" basis
    v1, s1, u1 = gas.v, gas.s, gas.u
    gas.SV = s1, v1 / CR
    u2, v2 = gas.u, gas.v
    if equilibrium:
        gas.UV = u2, v2
        gas.equilibrate('UV')
    else:
        gas.TDX = 2000.0, 1.0 / v2, frozen_products(lam)
        # same absolute internal energy (formation + sensible) as the charge
        gas.UV = u2, v2
    T3, p3, u3, s3 = gas.T, gas.P, gas.u, gas.s
    if equilibrium:
        gas.SV = s3, v1
        gas.equilibrate('SV')
    else:
        gas.SV = s3, v1
    u4 = gas.u
    w = (u3 - u4) - (u2 - u1)                   # J per kg mixture
    return w * m_mix / m_air, T3, p3 / 1e5      # J per kg air

if __name__ == '__main__':
    print('lambda | W_equilibrium kJ/kg-air  T3 K  p3 bar | W_frozen kJ/kg-air  T3 K | ratio eq/frozen')
    for lam in (0.80, 0.88, 0.92, 0.96, 1.00, 1.10):
        we, te, pe = run(lam, True)
        wf, tf, pf = run(lam, False)
        print('%.2f   | %8.1f  %6.0f %6.1f | %8.1f  %6.0f | %.3f' % (lam, we / 1e3, te, pe, wf / 1e3, tf, we / wf))
