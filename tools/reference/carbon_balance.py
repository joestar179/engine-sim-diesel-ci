"""Cycle fuel consumption from certification emissions (carbon balance).

Fuel carbon leaves the engine as CO2, CO and HC. Given brake-specific
emissions (g/kWh) over a weighted steady-state cycle, the brake-specific fuel
consumption over the same cycle is

    bsfc = 12.011 * (CO2/44.010 + CO/28.010 + HC/13.875) / w_c

(HC on the C1H1.85 basis of 40 CFR 1065). The mean mixture follows from the
CO share of the exhaust carbon with water-gas-shift equilibrium
(K = [CO][H2O]/([CO2][H2]) = 3.5, Heywood, ICE Fundamentals, sec. 4.9).
The unburned chemical energy in CO, H2 and HC gives the combustion efficiency.

Usage: python carbon_balance.py   (prints the table for the families below)
"""
import math

# Fuel: CARB LEV III E10 certification gasoline (ethanol 9.6-10.0 vol%,
# oxygen ~3.5 wt%). Hydrocarbon part CH1.87, w_c 0.866, LHV 43.0 MJ/kg;
# ethanol w_c 0.5214, LHV 26.8 MJ/kg; ethanol mass fraction 0.101 (from O2 wt%).
E10 = dict(name='LEV III E10', x_eth=0.101, wc_hc=0.866, lhv_hc=43.0, h_c=1.87)
# 40 CFR 1065.710 E0 gasoline (Tier 2 style indolene): w_c 0.866, LHV 43.0.
E0 = dict(name='E0 cert', x_eth=0.0, wc_hc=0.866, lhv_hc=43.0, h_c=1.87)

def fuel_props(f):
    x = f['x_eth']
    wc = (1 - x) * f['wc_hc'] + x * 0.5214
    lhv = (1 - x) * f['lhv_hc'] + x * 26.8
    return wc, lhv

def lambda_from_co_share(co_share, h_c=1.87, o_c=0.0, K=3.5):
    """Equivalence lambda from the CO fraction of exhaust carbon (rich side)."""
    a = 1.0 - co_share                       # CO2 per C
    # WGS: (1 - a) * h2o = K * a * h2, with h2 + h2o = h_c / 2
    h2o = K * a * (h_c / 2) / ((1 - a) + K * a)
    h2 = h_c / 2 - h2o
    o_needed = 2 * a + (1 - a) + h2o - o_c   # O atoms in products minus fuel O
    o_stoich = 2 + h_c / 2 - o_c
    return o_needed / o_stoich, h2 / (1 - a) if a < 1 else 0.0

def analyse(label, co2, co, hcnox, f, hc_share=(0.6, 0.8, 0.95)):
    wc, lhv = fuel_props(f)
    out = []
    for s in hc_share:
        hc = hcnox * s
        molC = co2 / 44.010 + co / 28.010 + hc / 13.875
        bsfc = 12.011 * molC / wc                      # g fuel / kWh
        energy = bsfc * lhv / 1000.0                   # MJ fuel / kWh
        co_share = (co / 28.010) / (co2 / 44.010 + co / 28.010)
        lam, h2_per_co = lambda_from_co_share(co_share, f['h_c'])
        h2 = (co / 28.010) * h2_per_co * 2.016          # g/kWh (equilibrium estimate)
        unburned = (co * 10.10 + h2 * 120.0 + hc * 43.0) / 1000.0   # MJ/kWh
        out.append((s, bsfc, energy, lam, 1 - unburned / energy, energy - unburned))
    print('%-34s fuel %-11s w_c %.4f LHV %.2f' % (label, f['name'], wc, lhv))
    for s, bsfc, e, lam, eta_c, rel in out:
        print('   HC share %.2f: BSFC %5.1f g/kWh  fuel energy %5.2f MJ/kWh  lambda %.3f  '
              'eta_comb %.3f  released %5.2f MJ/kWh  brake eff (fuel) %.3f' % (s, bsfc, e, lam, eta_c, rel, 3.6 / e))
    return out

if __name__ == '__main__':
    # Kohler family KHXS.7472GK (CH750, CH752, CV752). EPA certification data
    # (results before deterioration factor); EU Stage V type approval gives
    # CO2 887 (G1, 3060 rpm) and 971 (G2, 3600 rpm) g/kWh for the same family.
    analyse('KHXS.7472GK  G1 (EPA MY2022-26)', 889.0, 325.2, 7.49, E10)
    analyse('KHXS.7472GK  G2 (EPA MY2020-21)', 972.0, 252.5, 5.39, E10)
    analyse('KHXS.7472GK  G1 with EU CO2', 887.0, 325.2, 7.49, E10)
    analyse('KHXS.7472GK  G2 with EU CO2', 971.0, 252.5, 5.39, E10)
