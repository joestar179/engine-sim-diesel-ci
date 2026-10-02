"""Layer-3 calibration of a generated engine (CLAUDE.md 0e).

1. Spark: MBT (maximum WOT torque) at each timing-curve speed in range.
2. Breathing: port_cd bisected so that rated power x rating_cf = documented.
3. Report: knob vs its stated range; held-out curve vs documented curve
   (spec key "curve": [[rpm, Nm], ...]).

usage (from C:\\es\\run): python calibrate.py <spec.json> [--skip-mbt]
"""
import json, math, os, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(__file__))
import defaults as D

EXE = r'bin\engine-sim-dyno-sweep.exe'

def run(spec, rpms, overrides):
    args = ', '.join('%s: %s' % kv for kv in overrides.items())
    body = ('import "engine_sim.mr"\nimport "engines/validation/%s.mr"\nunits units()\nset_engine(%s(%s))\n'
            % (spec['node'], spec['node'], args))
    fd, path = tempfile.mkstemp(suffix='_main.mr', dir='assets'); os.write(fd, body.encode()); os.close(fd)
    try:
        out = subprocess.run([EXE, path, ','.join('%d' % r for r in rpms), '--settle', '4', '--measure', '2'],
                             capture_output=True, text=True).stdout
    finally:
        os.remove(path)
    rows = [l.split(',') for l in out.strip().splitlines()[1:] if l[:1].isdigit()]
    return {int(float(r[0])): (float(r[2]), float(r[3])) for r in rows}     # rpm -> (Nm, kW)

def main():
    spec = json.load(open(os.path.abspath(sys.argv[1])))
    spec_path = os.path.abspath(sys.argv[1])
    os.chdir('C:/es/run')          # tools resolve es/ and assets/ from the run layout
    skip_mbt = '--skip-mbt' in sys.argv
    cf = spec.get('rating_cf', 1.0)
    knobs = {}
    # 1. MBT spark per timing-curve sample speed
    lo_rpm, hi_rpm = spec.get('idle_rpm', 1000), spec.get('redline_rpm', spec['rated_rpm'])
    samples = [r for r in range(1000, 8000, 1000) if lo_rpm - 1000 < r <= hi_rpm + 1000]
    if not skip_mbt:
        for r in samples:
            test = min(max(r, lo_rpm), hi_rpm)
            best = None
            for adv in range(5, 56, 5):
                res = run(spec, [test], dict(knobs, **{'spark_%d' % r: '%d * units.deg' % adv}))
                t = res.get(test, (float('-inf'), 0))[0]
                if best is None or t > best[1]: best = (adv, t)
            knobs['spark_%d' % r] = '%d * units.deg' % best[0]
            print('MBT %5d rpm (run at %d): %d deg (%.1f N m)' % (r, test, best[0], best[1]))
    # 2. Breathing knob on rated power
    target = spec['rated_kw']
    rng = D.PORT_CD
    lo, hi = rng['low'], rng['high']
    for it in range(9):
        mid = 0.5 * (lo + hi)
        kw = run(spec, [spec['rated_rpm']], dict(knobs, port_cd='%.4f' % mid))[spec['rated_rpm']][1] * cf
        if kw < target: lo = mid
        else: hi = mid
    cd = 0.5 * (lo + hi)
    knobs['port_cd'] = '%.4f' % cd
    edge = cd < rng['low'] + 0.01 or cd > rng['high'] - 0.01
    print('port_cd = %.3f  (range %.2f-%.2f)%s' % (cd, rng['low'], rng['high'], '  ** AT RANGE EDGE: finding **' if edge else ''))
    # 2b. Tuning knob: intake lobe centre (later = more top-end) set so the
    # documented torque rise peak/rated is reproduced (uses only the two
    # rating points); port_cd re-bisected after each change.
    if 'peak_nm' in spec and 'peak_rpm' in spec:
        rated_nm = spec['rated_kw'] * 1000 / (spec['rated_rpm'] * math.pi / 30)
        rise_doc = spec['peak_nm'] / rated_nm
        cam = D.cam_class(spec['rated_rpm'])['intake_center']
        def rise(center):
            k = dict(knobs, intake_center='%.2f * units.deg' % center)
            r = run(spec, [spec['peak_rpm'], spec['rated_rpm']], k)
            return r[spec['peak_rpm']][0] / r[spec['rated_rpm']][0]
        lo_c, hi_c = cam['low'], cam['high']
        r_lo, r_hi = rise(lo_c), rise(hi_c)
        print('torque rise doc %.3f; sim at centre %.0f: %.3f, at %.0f: %.3f' % (rise_doc, lo_c, r_lo, hi_c, r_hi))
        if (r_lo - rise_doc) * (r_hi - rise_doc) > 0:
            center = lo_c if abs(r_lo - rise_doc) < abs(r_hi - rise_doc) else hi_c
            print('intake_center = %.1f  ** target outside range: finding **' % center)
        else:
            a_, b_ = lo_c, hi_c
            for it in range(7):
                m_ = 0.5 * (a_ + b_)
                if (rise(m_) - rise_doc) * (r_lo - rise_doc) > 0: a_ = m_
                else: b_ = m_
            center = 0.5 * (a_ + b_)
            print('intake_center = %.1f (range %.0f-%.0f)' % (center, lo_c, hi_c))
        knobs['intake_center'] = '%.2f * units.deg' % center
        lo, hi = rng['low'], rng['high']
        for it in range(9):
            mid = 0.5 * (lo + hi)
            kw = run(spec, [spec['rated_rpm']], dict(knobs, port_cd='%.4f' % mid))[spec['rated_rpm']][1] * cf
            if kw < target: lo = mid
            else: hi = mid
        cd = 0.5 * (lo + hi)
        knobs['port_cd'] = '%.4f' % cd
        edge = cd < rng['low'] + 0.01 or cd > rng['high'] - 0.01
        print('port_cd (re-bisected) = %.3f%s' % (cd, '  ** AT RANGE EDGE: finding **' if edge else ''))
    # 3. Held-out curve
    curve = spec.get('curve', [])
    if curve:
        res = run(spec, [c[0] for c in curve], knobs)
        print(' rpm   sim_Nm  doc_Nm   err')
        errs = []
        for rpm, doc in curve:
            sim = res[rpm][0] * cf
            errs.append(100 * (sim / doc - 1))
            print('%5d %7.2f %7.2f %+6.1f%%' % (rpm, sim, doc, errs[-1]))
        pk_sim = max(curve, key=lambda c: res[c[0]][0])[0]
        print('max |err| %.1f%%; peak-torque speed sim %d vs doc %d' % (max(abs(e) for e in errs), pk_sim, spec.get('peak_rpm', 0)))
    json.dump(knobs, open(os.path.splitext(spec_path)[0] + '.knobs.json', 'w'), indent=1)

if __name__ == '__main__':
    main()
