# P1 Kohler CH750 — supplement (2026-10-01)

## Sources checked

**Rehlko service manual 24 690 06 Rev. P** (CH18–CH25, CH620–CH750; 124 pp., `C:\es\run\assets`, not committed):

- Specification pp. 9–13: clearances and wear limits only.
  - Valves: minimum lift 8.07 mm (service minimum), seat 45°, guide ID 7.038–7.058 mm.
  - Journals: mains 40.913–40.935 mm, rod journal 35.955–35.973 mm; cylinder bore 82.988–83.013 mm (CH750).
  - Connecting rod: rod-to-crankpin side clearance 0.26–0.63 mm.
- No ignition timing values. p. 62: the CDI system has fixed timing set by flywheel-magnet position; the MDI system (fitted to the CH750-3005, per the pack's module 25 584 14-S) varies timing with engine speed; "no ... timing adjustments are necessary or possible".
- No cam timing, nominal lift, rod length, V angle, performance curve or fuel consumption.

**Rehlko/Kohler Gasoline Full Line brochure** (`Gasoline_Full_line.pdf`, 56 pp., `C:\es\run\assets`):

- p. 27 (table): CH750 27 hp (20.1 kW) @ 3600 rpm gross, peak torque 41.2 ft·lb (55.9 N·m) @ 3200, CR 9.4:1, rated per SAE J1940 gross; two-barrel carburettor (CH750 only, p. 26).
- **p. 26 (chart): maximum power and maximum torque curves, 2200–3600 rpm**, current J1940 rating. The legend labels the six curves CV620 / CV640 / CV680 / CV730 / CV740 / CV750 (the same chart is used for the vertical-shaft page 24). Grade **F** for CH750-3005 (family chart, CV legend on the CH page).

## Digitised CH750 curve (orange = 750, by legend swatch)

Extracted from the PDF vector paths (pypdf content stream; Bezier splines sampled), axes calibrated on the grid lines:

- power grid 10–28 hp, y = 71.1 → 253.9 pt;
- torque grid 25–45 ft·lb, y = 71.5 → 253.3 pt;
- speed 2200–3600 rpm over the curve span.

Reading uncertainty: the curves are drawn splines, not plotted points.

| rpm | Power kW | Torque N·m | Grade |
|---:|---:|---:|:---:|
| 2200 | 12.15 | 53.1 | F |
| 2400 | 13.34 | 53.9 | F |
| 2600 | 14.58 | 54.4 | F |
| 2800 | 15.79 | 54.7 | F |
| 3000 | 16.93 | 54.7 | F |
| 3200 | 18.01 | 54.5 | F |
| 3400 | 19.06 | 54.1 | F |
| 3600 | 20.17 | 53.4 | F |

**Consistency:**

- 3600 rpm power 20.17 kW vs table 20.1 kW (+0.3 %).
- Torque derived from the power curve agrees within ~1 % (52.7–53.9 N·m).
- The spline peak (54.7 @ 3000) is 2 % below the tabulated 55.9 @ 3200: use the tabulated peak for the peak check and the curve for the shape (±10 %).

## Cycle fuel consumption from certification emissions (grade F: family parent engine)

- **Family:** KHXS.7472GK (CH750, CH752, CV752), per the Rehlko "Published Engine CO2 Values" (EU Stage V, Regulation 2016/1628 Art. 43(4)): https://resources.rehlko.com/enginesus/pdf/Published_Engine_CO2_Values.pdf
  - G1 (3060 rpm): CO2 887 g/kWh, type approval e5*2016/1628*2016/1628SYB3/P*0129*00.
  - G2 (3600 rpm): CO2 971 g/kWh, type approval e5*2016/1628*2016/1628SRB3/P*0130*00.
- **EPA certification data** (Small NRSI interactive report, results before DF) for the same family:
  - MY2022–26: CO2 889, CO 325.2, HC+NOx 7.49 g/kWh (matches G1);
  - MY2020–21: CO2 972, CO 252.5, HC+NOx 5.39 g/kWh (matches G2);
  - test fuel CARB LEV III E10.
- **Cycle** (40 CFR 1054 Appendix B, ISO 8178 G1/G2): 100/75/50/25/10 % torque plus idle, weights 0.09/0.20/0.29/0.30/0.07/0.05.
- **Carbon balance** (`tools/reference/carbon_balance.py`):

| Cycle | BSFC (E10) | Fuel energy | λ | Combustion efficiency |
|---|---|---|---|---|
| G1 | 465 g/kWh | 19.24 MJ/kWh | 0.83 | 0.76 |
| G2 | 454 g/kWh | 18.76 MJ/kWh | 0.87 | 0.82 |

- **Check:** cycle fuel energy ±5 % (the BSFC criterion applied to the certified cycle). Cycle λ is a measured mixture (equipment input).

## Still unresolved

- MDI ignition advance values, cam timing and nominal lift, rod length, V angle: these remain C (bounded) inputs.
- MDI ignition advance values, cam timing and nominal lift, rod length, V angle: these remain C (bounded) inputs.
