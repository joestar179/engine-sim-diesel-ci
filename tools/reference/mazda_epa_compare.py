"""Run the Mazda SKYACTIV-G 2.0 script at EPA's measured steady-state points.

Each point: measured speed, spark timing, intake/exhaust cam phase and lambda
as script inputs; throttle bisected to the measured intake-manifold pressure
(dyno --map). Predictions compared with EPA: brake torque, fuel flow, BTE and
inlet air flow (sim airflow = metered fuel x 14.485 lambda, the premixed
intake's mass AFR).

Split (declared before any run; deterministic):
  points with positive torque, sorted by (speed, torque); every third point
  (index % 3 == 0) is CALIBRATE (breathing C values only, against airflow);
  the rest are HOLDOUT. Points with torque <= 5 N m are excluded (motoring /
  idle region, not a combustion check).

usage (from C:\\es\\run):
  python mazda_epa_compare.py <tier2|lev3> <out.csv> [--only calibrate|holdout]
         [--set name=value ...]   (extra script inputs, e.g. port_cd=0.6)
         [--jobs 4] [--boundary map|air]   (air: throttle set to the measured
                                           inlet air flow instead of MAP)
"""
import csv, os, subprocess, sys, tempfile
from concurrent.futures import ThreadPoolExecutor

DATA = 'C:/es/overlay/docs/reference/batch02/mazda_epa'
EXE = r'bin\engine-sim-dyno-sweep.exe'
LHV = {'tier2': 42.887, 'lev3': 41.763}           # MJ/kg (EPA fuel analyses)
AFR_STOICH_SIM = {'tier2': 14.485, 'lev3': 14.485 * 14.06 / 14.64}

def load(fuel):
    rows = list(csv.DictReader(open('%s/%s_steady_state.csv' % (DATA, fuel))))
    pts = [r for r in rows if float(r['Torque']) > 5.0]
    pts.sort(key=lambda r: (float(r['Speed']), float(r['Torque'])))
    for i, r in enumerate(pts):
        r['role'] = 'calibrate' if i % 3 == 0 else 'holdout'
        r['idx'] = i
    return pts

def run(fuel, r, extra, boundary='map'):
    args = ['spark_advance: %.3f * units.deg' % float(r['Spark Timing']),
            'intake_cam_phase: %.3f * units.deg' % float(r['Intake Cam Phase']),
            'exhaust_cam_phase: %.3f * units.deg' % float(r['Exhaust Cam Phase']),
            'lambda: %.4f' % float(r['Exhaust Lambda'])]
    if fuel == 'lev3':
        args.append('fuel: mazda_lev3_fuel()')
    args += ['%s: %s' % tuple(kv.split('=', 1)) for kv in extra]
    body = ('import "engine_sim.mr"\nimport "engines/validation/mazda_skyactiv_g20.mr"\n'
            'units units()\nset_engine(mazda_skyactiv_g20(%s))\n' % ', '.join(args))
    fd, path = tempfile.mkstemp(suffix='_main.mr', dir='assets')
    os.write(fd, body.encode()); os.close(fd)
    try:
        if boundary == 'air':
            # Measured inlet air -> metered fuel at this point's lambda.
            target = ['--fuel', '%.4f' % (float(r['Inlet Air Flow']) / (AFR_STOICH_SIM[fuel] * float(r['Exhaust Lambda'])))]
        else:
            target = ['--map', '%.2f' % float(r['Intake Manifold Press'])]
        out = subprocess.run([EXE, path, '%.0f' % float(r['Speed'])] + target +
                             ['--settle', '4', '--measure', '2', '--start-mode', 'dyno'], capture_output=True, text=True).stdout
    finally:
        os.remove(path)
    v = out.strip().splitlines()[-1].split(',')
    if len(v) < 9 or not v[0][0].isdigit():
        return None
    torque, kw, fuel_gs = float(v[2]), float(v[3]), float(v[4])
    lam = float(r['Exhaust Lambda'])
    air = fuel_gs * AFR_STOICH_SIM[fuel] * lam
    bte = kw / (fuel_gs * LHV[fuel]) * 100 if fuel_gs > 0 else 0.0
    return dict(torque=torque, fuel=fuel_gs, air=air, bte=bte, map=float(v[8]) + 101.325, control=float(v[1]))

def main():
    fuel, out = sys.argv[1], sys.argv[2]
    only = None; extra = []; jobs = 4; boundary = 'map'
    a = sys.argv[3:]
    for i, x in enumerate(a):
        if x == '--only': only = a[i + 1]
        if x == '--set': extra.append(a[i + 1])
        if x == '--jobs': jobs = int(a[i + 1])
        if x == '--boundary': boundary = a[i + 1]
    pts = [p for p in load(fuel) if only is None or p['role'] == only]
    with ThreadPoolExecutor(max_workers=jobs) as ex:
        res = list(ex.map(lambda p: run(fuel, p, extra, boundary), pts))
    with open(out, 'w', newline='') as fo:
        w = csv.writer(fo)
        w.writerow(['idx', 'role', 'speed', 'map_meas', 'map_sim', 'spark', 'icam', 'ecam', 'lambda',
                    'torque_meas', 'torque_sim', 'fuel_meas', 'fuel_sim', 'air_meas', 'air_sim',
                    'bte_meas', 'bte_sim'])
        for p, s in zip(pts, res):
            if s is None: continue
            w.writerow([p['idx'], p['role'], p['Speed'], p['Intake Manifold Press'], '%.2f' % s['map'],
                        p['Spark Timing'], p['Intake Cam Phase'], p['Exhaust Cam Phase'], p['Exhaust Lambda'],
                        p['Torque'], '%.2f' % s['torque'], p['Fuel Meter Flow'], '%.4f' % s['fuel'],
                        p['Inlet Air Flow'], '%.3f' % s['air'], p['BTE'], '%.2f' % s['bte']])
    print('wrote', out, len([s for s in res if s]), 'of', len(pts))

if __name__ == '__main__':
    main()
