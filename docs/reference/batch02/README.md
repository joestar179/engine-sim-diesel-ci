# Reference batch 02 — P3 Mazda SKYACTIV-G 2.0 (13:1) and P4 GM 4.3 L LV3 V6 (EPA NVFEL)

Review 2026-10-01.

## P3 — 2014 Mazda SKYACTIV-G 2.0 L, 13:1 (validation engine, typical 2 L inline)

The pack (`P3_*`) grades per-point spark, cam phase, λ, MAP and airflow as U. **Superseded:** the EPA 2018 test-data workbooks contain them.
- Tier 2: `2014-mazda-2.0l-skyactiv-engine-tier-2-fuel-test-data-package-03-29-18.zip`
- LEV III: `2014-mazda-2-0l-skyactiv-engine-lev-3-fuel-test-data-package-03-29-18.zip`
- Local copies in `C:\es\run\assets\epa\mazda_skyactiv_2014\`.

Workbook `4- ... Test Data.xlsx`:
- Sheet "Steady State" (Tier 2 ~138 rows, LEV III ~202 rows): Speed, Torque, BMEP, Cell Temp/Press, BSFC, BTE, Fuel Meter Flow, Injector Fuel Flow, Fuel Rail Press, Spark Timing, Coolant/Oil Temp, **Inlet Air Flow**, Intake Manifold Press/Temp, Exhaust Port Temp cyl 1-4, Catalyst Inlet Press/Temp, Exhaust Lambda, Intake Cam Phase, Exhaust Cam Phase, Valve Overlap.
- Sheet "Max Torque Sweep" (measured WOT).
- Sheet "Min Torque Sweep" (motoring/minimum torque).
- Plus the test report (docx) and fuel analysis reports 23945 / 24350.

**Still U (simulator inputs):**
- valve head diameters, lifts, durations and base centrelines;
- intake throttle bore, plenum and runners;
- 4-2-1 exhaust lengths and diameters;
- chamber volume and piston cavity;
- bearing widths.

## P4 — 2014 Chevrolet Silverado 4.3 L EcoTec3 LV3 V6 (validation engine, V config, DI, cylinder deactivation)

Pack `LV3_validation_grade_reference_pack.md`; raw CSVs in `C:\es\run\assets\epa\gm_lv3_2014\LV3_validation_grade_complete.zip`.

Raw steady points:
- Tier 2: 109.
- LEV III: 174 (148 normal, 26 cylinder-deactivation active), plus 32 transmission-coupled low-speed points.
- Per point: spark, MAP, exhaust pressure/temperature per bank, λ per bank, cam phase (single phaser), measured fuel flow.
- No airflow channel and no cylinder-pressure traces released. The WOT curve is GM's published curve, not a measurement.

**Still U:** all engine geometry. The pack refers to a "prior LV3 closure pack" that was not supplied.

**Simulator capability gaps:**
- cylinder deactivation (26 points; out of scope until modelled);
- direct-injection charge cooling (both engines);
- per-point cam phase, settable per run via camshaft advance (Mazda: separate intake/exhaust; LV3: single phaser).
