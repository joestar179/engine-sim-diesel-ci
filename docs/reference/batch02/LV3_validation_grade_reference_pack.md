# 2014 Chevrolet 4.3L EcoTec3 LV3 - Validation-Grade Reference Pack

**Status: VALIDATION-GRADE for the explicitly bounded steady-state domains below.**

This pack is built directly from the four EPA packages supplied by the user. Raw measured test points are kept separate from EPA's generated ALPHA maps and from EPA's explicitly estimated extension points.

## Configuration lock

- Vehicle: **2014 Chevrolet Silverado**
- VIN: **1GCNCPEH2EZ171727**
- Engine: **4.3L EcoTec3 LV3 V6**
- Rated power: **285 hp (213 kW) @ 5300 rpm**
- Rated torque: **305 lb-ft (413 Nm) @ 3900 rpm**
- Test lab: **US EPA NVFEL**
- Controls: production ECU/calibration retained by tethering engine harness to the donor vehicle.

## What is now actually available

- Tier 2 steady-state raw points: **109**
- LEV III steady-state raw points: **174**
- LEV III CDA-active raw points: **26**
- LEV III CDA-inactive raw points: **148**
- LEV III transmission-coupled low-speed points: **32**
- Every steady-state point includes measured torque and fuel flow. The released sheets also include spark timing, manifold pressure, exhaust pressures/temperatures, lambda and cam phase.

## Validation/calibration split

| Role | Points | Purpose |
|---|---:|---|
| VALIDATE_LEV3_CROSSFUEL | 148 | All LEV-III non-CDA points after Tier-2 calibration is frozen. |
| CALIBRATE_TIER2 | 62 | Tier-2 positive-load calibration subset only. |
| VALIDATE_TIER2_HOLDOUT | 39 | Independent same-fuel positive-load holdout, including the highest-load point at every tested speed. |
| VALIDATE_LOW_SPEED_DRIVELINE | 32 | Transmission-coupled low-speed/idle points. |
| VALIDATE_CDA_CROSSFUEL | 26 | All measured LEV-III CDA-active points; validates cylinder-deactivation effect. |
| VALIDATE_MIN_TORQUE_OR_NONPOWER | 8 | Minimum/negative-torque envelope; kept out of combustion calibration. |

The split is deterministic and listed row-by-row in `validation_split.csv`. It deliberately avoids using the same observations both to tune and validate the same subsystem.

## Independent measured inputs / state variables

- **Calibration to a standard:** speed, torque, barometric pressure, fuel-meter flow, oil pressure, fuel-feed pressure, intake-manifold pressure, both exhaust-manifold pressures.
- **Verification to a standard:** cell/coolant/oil/intake-manifold temperatures and exhaust/post-catalyst temperatures.
- **Reference only:** CDA state, average spark timing, both exhaust lambda channels.
- **Calculated by EPA:** BMEP, BSFC, BTE and cam phase.

### Fuel properties used by EPA

| Fuel | Density kg/L @15C | LHV MJ/kg | AKI | RON | MON | Ethanol vol% |
|---|---:|---:|---:|---:|---:|---:|
| Tier 2 FTAG 24693 | 0.74571 | 42.889 | 92.3 | 96.2 | 88.4 | 0 |
| LEV III FTAG 24670 | 0.74818 | 41.824 | 88.425 | 92.3 | 84.5 | 9.34 |

## Numerical integrity check

- Tier-2 BSFC recalculation maximum absolute difference: **2.274e-13 g/kWh**.
- Tier-2 BTE recalculation maximum absolute difference: **7.105e-15 percentage points**.
- LEV-III BSFC recalculation maximum absolute difference: **2.274e-13 g/kWh**.
- LEV-III BTE recalculation maximum absolute difference: **8.545e-04 percentage points**.

These near-zero discrepancies confirm that the released fuel-flow, speed and torque values reproduce EPA's BSFC/BTE calculations.

## Raw-data performance anchors

- Tier-2 highest measured torque in the released steady-state sheet: **373.2 N·m @ 3499 rpm**. This is a measured dyno point, not the published WOT rating curve.
- LEV-III highest measured torque: **363.9 N·m @ 3499 rpm**.
- Tier-2 best measured BTE: **35.946% @ 2500 rpm / 314.7 N·m**, BSFC **233.509 g/kWh**.
- LEV-III best measured BTE: **35.738% @ 2499 rpm / 319.5 N·m**, BSFC **240.857 g/kWh**.

## ALPHA map provenance - critical restriction

EPA's ALPHA map is useful as a complete operating-envelope reference, but it is **not a pure measured dataset**.

- Each supplied map has **356 rows**, with a complete CDA-inactive fuel map and **90 valid CDA-active cells**.
- EPA explicitly added **estimated high-load fuel points** where raw data were unavailable.
- EPA imported a **published GM WOT curve** as the maximum-torque boundary rather than measuring that entire curve in this dataset.
- Tier-2 CDA behavior was generated using the relative CDA fuel-consumption reduction measured on LEV-III fuel because Tier-2 CDA measurements were not collected.
- The map spreadsheets therefore remain **secondary reference/model data**. Validation scoring must use the raw test CSVs, not interpolated/estimated map cells.

The exact estimated extension points are isolated in `alpha_estimated_high_load_points_EXCLUDE_FROM_VALIDATION.csv`; the published WOT boundary is isolated in `published_wot_curve_used_by_alpha.csv`.

## Allowed validation claims

- steady-state brake torque/power at withheld raw dyno points
- BSFC and BTE at withheld raw dyno points
- cross-fuel response using LEV III points after parameters are frozen on Tier 2
- cylinder-deactivation efficiency effect using 26 raw CDA-active LEV III points
- low-speed / idle fuel and torque behavior using transmission-coupled LEV III points
- manifold pressure, exhaust pressure/temperature, lambda, spark and cam-phase state consistency checks

## Claims that remain prohibited / not closed

- numeric cylinder-pressure trace validation: instrumented but traces are not released in the spreadsheet package
- direct measured airflow validation: LFE existed in test cell but airflow is not a released Test Data.xlsx field
- transient rotational-inertia validation: no production engine/flywheel/accessory inertia set is supplied
- injector spray/plume validation: nozzle geometry not supplied
- procedural-audio spectral validation: no controlled microphone recording in package
- using ALPHA estimated high-load or interpolated cells as if they were measured validation points

### Cylinder pressure clarification

The EPA report states that the engine was instrumented with Kistler in-cylinder pressure transducers and that combustion data were logged. **The released Test Data.xlsx files do not contain the crank-angle pressure traces.** Therefore the public package supports the existence and methodology of pressure measurement, but it does not support a numeric pressure-trace validation claim.

### Airflow clarification

The test-cell equipment list includes a laminar-flow element for air-flow measurement, but `Test Data.xlsx` does not expose an airflow channel. Airflow may not be treated as a directly released numeric validation target from this package.

## Recommended validation sequence for Engine Simulator

1. Lock geometry, compression ratio, documented cam/valve data and the exact test fuel.
2. Calibrate only against the 62 Tier-2 `CALIBRATE_TIER2` points, using measured fuel flow and measured boundary/control states.
3. Freeze combustion/friction/gas-exchange calibration.
4. Score the 39 Tier-2 positive-load holdouts and 8 minimum-torque holdouts.
5. Without retuning physical parameters, apply LEV-III fuel properties and each point's measured spark/cam/lambda/MAP state and score all 148 non-CDA LEV-III points.
6. Enable generic CDA logic and score the 26 CDA-active LEV-III points.
7. Finally check the 32 transmission-coupled low-speed points. Do not use these to disguise an incorrect engine-only idle model because driveline loading is part of the test setup.

## Included files

- `raw_tier2_steady_state.csv` - normalized EPA raw Tier-2 data.
- `raw_lev3_steady_state.csv` - normalized EPA raw LEV-III data.
- `raw_lev3_transmission_low_speed.csv` - transmission-coupled low-speed data.
- `validation_split.csv` - exact calibration/holdout assignment.
- `cross_fuel_matched_pairs.csv` - 84 nearest matched operating pairs for cross-fuel inspection.
- `alpha_tier2_generated_map.csv` and `alpha_lev3_generated_map.csv` - EPA generated maps, explicitly secondary.
- `alpha_estimated_high_load_points_EXCLUDE_FROM_VALIDATION.csv` - EPA estimated points isolated.
- `published_wot_curve_used_by_alpha.csv` - published WOT boundary imported by EPA.
- `data_dictionary.csv` - EPA parameter definitions and calibration status.
- `LV3_validation_grade_summary.json` - machine-readable audit summary.
- `source_manifest.json` - SHA-256 hashes of the four user-supplied EPA ZIP packages.
