"""Build the TCC-III motored engine script from the extracted UM data
(docs/reference/batch03/tcc3). Hardware-only inputs; measured valve flow
replaces the port_cd knob (tier 2)."""
import csv, math

D = 'C:/es/overlay/docs/reference/batch03/tcc3/'
OUT = 'C:/es/overlay/patches/forced_induction_v1/assets/engines/validation/tcc3.mr'
TEMPLATE = 'C:/es/overlay/tools/reference/tcc3_template.mr'

lift = list(csv.DictReader(open(D + 'tcc3_valve_lift.csv')))


def lobe(col_ca, col_l):
    pts = [(float(r[col_ca]), float(r[col_l])) for r in lift if r[col_ca] != '' and r[col_l] != '']
    peak = max(pts, key=lambda p: p[1])[0]
    rows = ['        .add_sample(%.2f * units.deg, %.4f * units.mm)' % ((ca - peak) / 2.0, l) for ca, l in pts]
    return rows, peak


cd = {}
for r in csv.DictReader(open(D + 'tcc3_valve_flow_coefficients.csv')):
    cd.setdefault(r['valve'], []).append((float(r['L_over_D']), float(r['Cd_forward'])))


def flow(v):
    tab = cd[v]
    dref = 25.4
    area = math.pi / 4 * dref ** 2
    rows = []
    for thou in range(0, 476, 25):
        ld = thou * 0.0254 / dref
        c = tab[-1][1]
        for (x0, y0), (x1, y1) in zip(tab, tab[1:]):
            if x0 <= ld <= x1:
                c = y0 + (y1 - y0) * (ld - x0) / (x1 - x0)
                break
        rows.append('        .add_flow_sample(%d, %.2f * cd)' % (thou, 0.2275 * c * area))
    return rows


il, ipk = lobe('intake_CA_firingTDC', 'intake_lift_mm')
el, epk = lobe('exhaust_CA_firingTDC', 'exhaust_lift_mm')
t = open(TEMPLATE).read()
t = (t.replace('@INTAKE_FLOW@', '\n'.join(flow('intvalve')))
      .replace('@EXHAUST_FLOW@', '\n'.join(flow('exhvalve')))
      .replace('@INTAKE_LOBE@', '\n'.join(il))
      .replace('@EXHAUST_LOBE@', '\n'.join(el))
      .replace('@EXHAUST_CENTER@', '%.2f' % (720.0 - 606.8))
      .replace('@INTAKE_CENTER@', '%.2f' % 114.8))
open(OUT, 'w').write(t)
print('written', OUT, 'intake peak CA(firing) %.0f exhaust %.0f' % (ipk, epk))
