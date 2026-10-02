# TCC-III extraction from the Motored Full View archive (2026-10-03)

Source: `research data/Motored_Full_FOV-Schiffmann.zip` (31 GB, 1.22 M files, almost all PIV). Extracted only what the simulator needs: 19 files, 37 MB, raw copies in `research data/TCCIII_extract/`. Script: session scratchpad `tcc3_extract.py`.

| File | Content | Grade |
|---|---|---|
| `tcc3_valve_lift.csv` | Intake / exhaust lift vs crank angle, 2° steps, peak 8.89 mm. Columns in the file's firing-TDC convention and in CA aTDCe (+360). Peak lift at 114.8 / 606.8 aTDCe, matching the README | D (`TCCIII-valve_lift_updated_20130806.xlsx`) |
| `tcc3_valve_flow_coefficients.csv` | Cd vs L/D (0-0.433), forward and reverse, both valves, reference diameter 25.4 mm | D as used in the UM GT-Power model (measured-flow source not stated; treat as F-hardware) |
| `tcc3_gt_pipe_elements.csv` | Every GT-Power pipe / flow-split element: diameters in / out, length, bend radius / angle | MODEL-DERIVED representation. Physical drawings in the README agree (25.4 mm runners / ports) |
| `tcc3_motored_{0800rpm_95kPa,1300rpm_95kPa,1300rpm_40kPa}_ensemble.csv` | 0.5° ensemble traces: intake plenum, intake port, cylinder, exhaust port, exhaust plenum, cylinder volume. Designated test plus the mean of all tests | D |
| `tcc3_motored_scalars.csv` | Per designated test: plenum / port pressures and temperatures, total air flow, rpm, IMEP, peak pressure, cylinder outer surface temperature; StdDev rows labelled (40 kPa) | D |

**Key scalars (designated tests):**

| Condition | Air g/s | Intake plenum T | Cylinder surface T | IMEP kPa | Peak p kPa |
|---|---|---|---|---|---|
| 800 rpm / 95 kPa | 3.503 | 47.3 °C | 36.4 °C | −42.05 | 1801.8 |
| 1300 rpm / 95 kPa | 5.899 | 49.6 °C | 38.2 °C | −37.85 | 1957.7 |
| 1300 rpm / 40 kPa | 2.115 | 42.4 °C | 41.9 °C | −18.96 | 811.9 |

**Intake path** (GT model, plenum → valve):
- plenum tank → flange contraction 101.6 → 25.4 mm in six steps (~33 mm long);
- runner 25.4 mm x 119.7 mm (90° bend, r 76.2);
- sensor section 28.2 mm; tuning spacer 4.76 mm;
- port 25.4 mm x 94.2 mm (90° bend, r 60);
- valves: 2 (1 intake, 1 exhaust).

The exhaust mirrors it (port 94.2, sensor 28.2, runner 119.7, flange 28.6 x 19.1) into the exhaust plenum.

**Discrepancies (recorded, not resolved):**
- Connecting rod: GT model 234.95 mm vs README 231.0 mm.
- Plenum: the GT model uses a round pipe of d 177.9 x 609.6 mm (15.2 L); the README gives a rectangular 609.6 x 101.6 x 177.8 mm section (11.0 L) plus a non-prismatic lower transition.
- GT wall temperature parameter 323 K vs the measured cylinder outer surface 36-42 °C.
