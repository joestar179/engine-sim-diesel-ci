"""Run the 40 CFR 1054 Appendix B / ISO 8178 G1-G2 six-mode cycle in the simulator.

Modes at the test speed: 100, 75, 50, 25, 10 % of the full-load torque at that
speed, then idle (governed idle speed, torque < 5 % of mode 1), weighted
0.09 / 0.20 / 0.29 / 0.30 / 0.07 / 0.05. Cycle results are
sum(w * fuel) / sum(w * power), as in 40 CFR 1054.505.

usage: python cert_cycle.py <script.mr> <test_rpm> <idle_rpm> <fuel_LHV_MJ_kg>
       (run from C:\\es\\run; uses bin\\engine-sim-dyno-sweep.exe)
"""
import subprocess, sys
from concurrent.futures import ThreadPoolExecutor

EXE = r'bin\engine-sim-dyno-sweep.exe'
WEIGHTS = [0.09, 0.20, 0.29, 0.30, 0.07, 0.05]
FRACTIONS = [1.0, 0.75, 0.50, 0.25, 0.10]

def run(script, rpm, torque=None):
    cmd = [EXE, script, '%d' % rpm, '--settle', '6', '--measure', '4']
    if torque is not None:
        cmd += ['--torque', '%.4f' % torque]
    out = subprocess.run(cmd, capture_output=True, text=True, check=True).stdout.strip().splitlines()
    v = out[-1].split(',')
    return dict(rpm=float(v[0]), control=float(v[1]), torque=float(v[2]), kW=float(v[3]),
                fuel=float(v[4]) * 3600.0, burned=float(v[6]) * 3600.0)   # g/h

def main():
    script, rpm, idle, lhv = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), float(sys.argv[4])
    wot = run(script, rpm)
    t100 = wot['torque']
    jobs = [(rpm, f * t100) for f in FRACTIONS[1:]] + [(idle, 0.02 * t100)]
    with ThreadPoolExecutor(max_workers=4) as ex:
        rest = list(ex.map(lambda j: run(script, j[0], j[1]), jobs))
    modes = [wot] + rest
    print('mode  rpm   control  torque_Nm  power_kW  fuel_g_h  burned_g_h  bsfc_g_kWh')
    for i, m in enumerate(modes):
        print('%4d %5.0f  %7.4f  %9.2f  %8.3f  %8.1f  %10.1f  %10.1f' % (
            i + 1, m['rpm'], m['control'], m['torque'], m['kW'], m['fuel'], m['burned'],
            m['fuel'] / m['kW'] if m['kW'] > 0.05 else float('nan')))
    W = sum(w * max(0.0, m['kW']) for w, m in zip(WEIGHTS, modes))
    F = sum(w * m['fuel'] for w, m in zip(WEIGHTS, modes))
    B = sum(w * m['burned'] for w, m in zip(WEIGHTS, modes))
    bsfc = F / W
    print('weighted: BSFC %.1f g/kWh  fuel energy %.2f MJ/kWh  released energy %.2f MJ/kWh  '
          'burned/inducted %.3f' % (bsfc, bsfc * lhv / 1000.0, B / W * lhv / 1000.0, B / F))

if __name__ == '__main__':
    main()
