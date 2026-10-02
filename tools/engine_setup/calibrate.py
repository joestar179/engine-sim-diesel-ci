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
COARSE = '--coarse' in sys.argv          # quick pass: fewer steps, shorter windows
SETTLE, MEASURE = ('2', '1') if COARSE else ('4', '2')

def run(spec, rpms, overrides):
    # Starting can fail for a given crank throttle (V-twin at 0.3, restricted
    # engines at 0); a point counts as non-running only if both fail.
    res = None
    for cc in ('0.0', '0.3'):
        r = _run(spec, rpms, overrides, cc)
        res = r if res is None else {k: (res.get(k) or r.get(k)) for k in set(res) | set(r)}
        if all(res.get(x) for x in rpms): break
    return res

def _run(spec, rpms, overrides, crank_control):
    args = ', '.join('%s: %s' % kv for kv in overrides.items())
    body = ('import "engine_sim.mr"\nimport "engines/validation/%s.mr"\nunits units()\nset_engine(%s(%s))\n'
            % (spec['node'], spec['node'], args))
    fd, path = tempfile.mkstemp(suffix='_main.mr', dir='assets'); os.write(fd, body.encode()); os.close(fd)
    try:
        out = subprocess.run([EXE, path, ','.join('%d' % r for r in rpms), '--settle', SETTLE, '--measure', MEASURE,
                              '--crank-control', crank_control, '--start-mode', 'dyno'],
                             capture_output=True, text=True).stdout
    finally:
        os.remove(path)
    rows = [l.split(',') for l in out.strip().splitlines()[1:] if l[:1].isdigit()]
    if not rows:
        raise RuntimeError('no result (script compile failure?) for %s %s' % (spec['node'], overrides))
    # rpm -> (Nm, kW); a non-running engine (WOT torque <= 0) is returned as None
    return {int(float(r[0])): ((float(r[2]), float(r[3])) if float(r[2]) > 0 else None) for r in rows}

def bisect_cd(spec, knobs, target, cf, rng):
    """Breathing knob on rated power. Non-running cases are reported, never
    used as 'too low'. Returns (cd, note)."""
    lo, hi = rng['low'], rng['high']
    def kw(cd):
        r = run(spec, [spec['rated_rpm']], dict(knobs, port_cd='%.4f' % cd)).get(spec['rated_rpm'])
        return None if r is None else r[1] * cf
    k_lo, k_hi = kw(lo), kw(hi)
    if k_lo is None or k_hi is None:
        return (lo if k_hi is not None else hi), 'engine not running at a range end (kW lo=%s hi=%s): finding' % (k_lo, k_hi)
    if k_lo > target: return lo, 'target below range (%.2f kW at cd %.2f): finding' % (k_lo, lo)
    if k_hi < target: return hi, 'target above range (%.2f kW at cd %.2f): finding' % (k_hi, hi)
    for it in range(5 if COARSE else 9):
        mid = 0.5 * (lo + hi)
        k = kw(mid)
        if k is None: return mid, 'engine not running inside range: finding'
        if k < target: lo = mid
        else: hi = mid
    return 0.5 * (lo + hi), 'in range'

def main():
    spec = json.load(open(os.path.abspath(sys.argv[1])))
    spec_path = os.path.abspath(sys.argv[1])
    os.chdir('C:/es/run')          # tools resolve es/ and assets/ from the run layout
    skip_mbt = '--skip-mbt' in sys.argv
    cf = spec.get('rating_cf', 1.0)
    knobs = {}
    # 0. Variable cam timing: WOT phase per speed for maximum torque within
    # the documented authority (ECU WOT criterion); intake first, then exhaust.
    ai, ae = spec.get('vvt_intake_authority', 0.0), spec.get('vvt_exhaust_authority', 0.0)
    import generate
    V, _ = generate.resolve(spec)
    OVERLAP_CAP = spec.get('overlap_cap_deg', 70.0)   # C: Mazda EPA WOT phases reach ~62 deg at 0.050 in (D)
    lo0, hi0 = spec.get('idle_rpm', 1000), spec.get('redline_rpm', spec['rated_rpm'])
    documented = spec.get('vvt_phases', {})   # {rpm: [intake adv, exhaust ret]} (D) — used, not optimised
    for r, (i, e) in documented.items():
        knobs['icam_%s' % r] = '%.1f * units.deg' % i
        knobs['ecam_%s' % r] = '%.1f * units.deg' % e
        print('VVT %s rpm documented: icam %.1f ecam %.1f' % (r, i, e), flush=True)
    if (ai or ae) and '--skip-vvt' not in sys.argv:
        for r in [x for x in range(1000, 8000, 1000) if lo0 - 1000 < x <= hi0 + 1000 and str(x) not in documented]:
            test = min(max(r, lo0), hi0)
            for key, auth in (('icam_%d' % r, ai), ('ecam_%d' % r, ae)):
                if not auth: continue
                best = None
                steps = 3 if COARSE else 6
                for k in range(steps):
                    ph = auth * k / (steps - 1.0)
                    # Valve overlap at 0.050 in (deg) for this phase pair; skip
                    # combinations beyond the plausibility cap.
                    ic = float(knobs.get('icam_%d' % r, '0').split()[0]) if key.startswith('e') else ph
                    ec = ph if key.startswith('e') else float(knobs.get('ecam_%d' % r, '0').split()[0])
                    ivo = (V['intake_center'] - ic) - V['intake_duration'] / 2.0
                    evc = -(V['exhaust_center'] - ec) + V['exhaust_duration'] / 2.0
                    if evc - ivo > OVERLAP_CAP: continue
                    res = run(spec, [test], dict(knobs, **{key: '%.1f * units.deg' % ph}))
                    t = (res.get(test) or (float('-inf'), 0))[0]
                    if best is None or t > best[1]: best = (ph, t)
                knobs[key] = '%.1f * units.deg' % best[0]
                print('VVT %-9s (run at %d): %.1f deg (%.1f N m)' % (key, test, best[0], best[1]))
    # 1. MBT spark per timing-curve sample speed
    lo_rpm, hi_rpm = spec.get('idle_rpm', 1000), spec.get('redline_rpm', spec['rated_rpm'])
    samples = [r for r in range(1000, 8000, 1000) if lo_rpm - 1000 < r <= hi_rpm + 1000]
    if not skip_mbt:
        for r in samples:
            test = min(max(r, lo_rpm), hi_rpm)
            best = None
            for adv in (range(5, 56, 10) if COARSE else range(5, 56, 5)):
                res = run(spec, [test], dict(knobs, **{'spark_%d' % r: '%d * units.deg' % adv}))
                t = (res.get(test) or (float('-inf'), 0))[0]
                if best is None or t > best[1]: best = (adv, t)
            knobs['spark_%d' % r] = '%d * units.deg' % best[0]
            print('MBT %5d rpm (run at %d): %d deg (%.1f N m)' % (r, test, best[0], best[1]))
    # 2. Breathing knob on rated power
    target = spec['rated_kw']
    rng = D.PORT_CD
    cd, note = bisect_cd(spec, knobs, target, cf, rng)
    knobs['port_cd'] = '%.4f' % cd
    print('port_cd = %.3f  (range %.2f-%.2f): %s' % (cd, rng['low'], rng['high'], note))
    # 2b. Tuning knob: intake lobe centre (later = more top-end) set so the
    # documented torque rise peak/rated is reproduced (uses only the two
    # rating points); port_cd re-bisected after each change.
    # (VVT engines: the phase optimisation above is the tuning; skip.)
    if 'peak_nm' in spec and 'peak_rpm' in spec and not (ai or ae):
        rated_nm = spec['rated_kw'] * 1000 / (spec['rated_rpm'] * math.pi / 30)
        rise_doc = spec['peak_nm'] / rated_nm
        cam = D.cam_class(spec['rated_rpm'])['intake_center']
        def rise(center):
            k = dict(knobs, intake_center='%.2f * units.deg' % center)
            r = run(spec, [spec['peak_rpm'], spec['rated_rpm']], k)
            if r[spec['peak_rpm']] is None or r[spec['rated_rpm']] is None: return float('nan')
            return r[spec['peak_rpm']][0] / r[spec['rated_rpm']][0]
        lo_c, hi_c = cam['low'], cam['high']
        r_lo, r_hi = rise(lo_c), rise(hi_c)
        print('torque rise doc %.3f; sim at centre %.0f: %.3f, at %.0f: %.3f' % (rise_doc, lo_c, r_lo, hi_c, r_hi))
        if (r_lo - rise_doc) * (r_hi - rise_doc) > 0:
            center = lo_c if abs(r_lo - rise_doc) < abs(r_hi - rise_doc) else hi_c
            print('intake_center = %.1f  ** target outside range: finding **' % center)
        else:
            a_, b_ = lo_c, hi_c
            for it in range(4 if COARSE else 7):
                m_ = 0.5 * (a_ + b_)
                if (rise(m_) - rise_doc) * (r_lo - rise_doc) > 0: a_ = m_
                else: b_ = m_
            center = 0.5 * (a_ + b_)
            print('intake_center = %.1f (range %.0f-%.0f)' % (center, lo_c, hi_c))
        knobs['intake_center'] = '%.2f * units.deg' % center
        cd, note = bisect_cd(spec, knobs, target, cf, rng)
        knobs['port_cd'] = '%.4f' % cd
        print('port_cd (re-bisected) = %.3f: %s' % (cd, note))
    # 3. Held-out curve
    curve = spec.get('curve', [])
    if COARSE and len(curve) > 6:
        idx = [round(i * (len(curve) - 1) / 5) for i in range(6)]
        curve = [curve[i] for i in sorted(set(idx))]
    if curve:
        res = run(spec, [c[0] for c in curve], knobs)
        print(' rpm   sim_Nm  doc_Nm   err')
        errs = []
        for rpm, doc in curve:
            sim = res[rpm][0] * cf if res[rpm] else float('nan')
            errs.append(100 * (sim / doc - 1))
            print('%5d %7.2f %7.2f %+6.1f%%' % (rpm, sim, doc, errs[-1]))
        pk_sim = max(curve, key=lambda c: (res[c[0]] or (-1, 0))[0])[0]
        print('max |err| %.1f%%; peak-torque speed sim %d vs doc %d' % (max(abs(e) for e in errs), pk_sim, spec.get('peak_rpm', 0)))
    json.dump(knobs, open(os.path.splitext(spec_path)[0] + '.knobs.json', 'w'), indent=1)

if __name__ == '__main__':
    main()
