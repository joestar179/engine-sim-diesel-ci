# P2 — Honda GX160 validation-grade data pack

## Scope

Target engine: **Honda GX160**, nominally 163 cc, 68 × 45 mm, single-cylinder OHV, float carburettor, air-cooled.

Purpose: validation/reference data for a 0-D / 1-D engine simulator.

### Grades

- **D** — exact physical configuration / directly reported measurement or stated configuration.
- **F** — Honda GX160 family / related generation; not proven to be the exact tested specimen.
- **R** — derived or digitised; derivation or digitisation basis is stated.
- **U** — not found.

### Configuration-control rule

Each study/test setup is kept separate. Values from different studies are **not merged silently**.

---

# 1. Study/configuration map

| ID | Physical configuration | Role |
|---|---|---|
| **S1 — Ragan et al. 2015** | Honda GX160, stock baseline CR 8.5:1; separate CR-modified and tuned-pipe builds; AVL DP80 dynamometer | Main brake-performance reference; exact spark and one full-load lambda point; cylinder-pressure figure; tuned pipe study |
| **S2 — Aprinaldi 2014** | Honda GX160, standard head plus milled-head variants; brake dynamometer + airbox/anemometer + volumetric fuel measurement | Same-setup air/fuel/load data, part-load/load-sweep reference |
| **S3 — Kocakulak et al. 2023** | Used/worn Honda GX160; gasoline baseline plus MEK blends; WOT dynamometer testing | Independent explicit full-load torque/power/SFC reference |
| **S4 — University of Liverpool MECH217** | Honda GX160 teaching dynamometer rig; DYNOmite + fuel flask + hot-film MAF | Experimental design matches required coupled WOT torque+fuel+air outputs; public numerical results not found |
| **F1 — older stock GX160 technical material** | Older 25° BTDC stock-Honda family / stock-component inspection | Rod, cam, valve lift, carburettor, port dimensions |
| **F2 — Honda service/assembly material** | GX160 / GX120–GX200 family | Valve diameters, valve clearance, stems/guides, float data |
| **F3 — Honda engine-application material** | GX120/GX160/GX200 installation requirements | Exhaust back-pressure limit |
| **P1 — Noga & Moskal 2024** | **WEIMA 168FA**, 163 cc GX160-like clone, not Honda | F-family known-point cylinder-pressure fallback only |
| **M1 — Kuchmacz & Noga 2025** | GT-Power model labelled GX160 | MODEL-DERIVED data only |

---

# 2. HARDWARE — A. Geometry

| Quantity | Value | Unit | Grade | Exact source |
|---|---:|---|:---:|---|
| Displacement, S1 | 163 | cm³ | D | Ragan et al. 2015, §2 Engine Characteristics. https://www.researchgate.net/publication/306407837_PARAMETERS_OPTIMIZATION_OF_MINIMOTOR |
| Bore, S3 | 68 | mm | D | Kocakulak et al. 2023, Table 1. https://dergipark.org.tr/tr/download/article-file/3285364 |
| Stroke, S3 | 45 | mm | D | Kocakulak et al. 2023, Table 1. https://dergipark.org.tr/tr/download/article-file/3285364 |
| Swept volume from 68 × 45 | 163.426 | cm³ | R | `π/4 × 68² × 45 / 1000` |
| Connecting-rod centre distance, older GX160 family | 83.845–84.392 | mm | R from F | GX160 Technical Manual, connecting-rod inspection dimensions; arithmetic below. https://studylib.net/doc/18655427/honda-gx-160---tech-manual-updated-november-28--2012 |
| Recommended older-family rod input | ≈84.0 | mm | F/R | Same F1 source; not measured on S1 specimen |
| Compression ratio, S1 stock | 8.5:1 | — | D | Ragan et al. 2015, §2 |
| Geometrically measured S1 compression ratio | U | — | U | Not found |
| Nominal clearance volume at 8.5:1 | 21.790 | cm³ | R | `163.426 / (8.5 - 1)` |
| Modified CR, S1 | 10:1; 12:1 | — | D | Ragan et al. 2015, §4 |
| Piston-pin offset, S1 | 2 | mm | D | Ragan et al. 2015, §2.3 |
| Cranking compression, S2 standard head | 5.9 | bar | D | Aprinaldi 2014, Chapter IV, Table 4.8; compression gauge at spark-plug hole |

### Connecting-rod derivation

Older GX160 stock inspection dimensions:

- big-end bore: **1.176–1.184 in**
- pin-end bore: **0.710 in reference**
- distance from bottom of pin bore to top of big-end bore: **2.3580–2.3755 in**

Therefore:

`L_min = 2.3580 + 0.710/2 + 1.176/2 = 3.3010 in = 83.845 mm`

`L_max = 2.3755 + 0.710/2 + 1.184/2 = 3.3225 in = 84.392 mm`

This is **R from F-family dimensional evidence**, not a direct S1 rod measurement.

---

# 3. HARDWARE — B. Valves and ports

## 3.1 Valve head / seat / stem / clearance

| Quantity | Value | Unit | Grade | Exact source |
|---|---:|---|:---:|---|
| Intake valve head diameter | 25 | mm | F | Honda GX160/GX200 Assembly Information. https://manualzz.com/doc/62037288/honda-gx160--gx200-assembly-information |
| Exhaust valve head diameter | 24 | mm | F | Same |
| Intake cold valve clearance | 0.15 ± 0.02 | mm | F | Honda GX120/GX160/GX200 UT2 Shop Manual, valve-clearance specification. https://lightbournequipment.com/pdfs/GX120%2C%20GX160%2C%20GX200%20UT2%20SHOP%20MANUAL.pdf |
| Exhaust cold valve clearance | 0.20 ± 0.02 | mm | F | Same |
| Intake valve stem OD | 5.468–5.480 | mm | F | Honda service information |
| Exhaust valve stem OD | 5.425–5.440 | mm | F | Honda service information |
| Valve guide ID | 5.500–5.512 | mm | F | Honda service information |
| Valve seat width | 0.70–0.90 | mm | F | Honda service information |
| Valve seat-circle diameter | U | mm | U | Not found |

## 3.2 Valve lift and crank-angle timing

Source: older GX160 Technical Manual camshaft inspection table, zero tappet clearance, degree wheel / established TDC.

URL: https://studylib.net/doc/18655427/honda-gx-160---tech-manual-updated-november-28--2012

| Tappet lift | Intake crank position | Exhaust crank position | Grade |
|---:|---|---|:---:|
| 1.27 mm / 0.050 in | 10.5–14° ATDC, rising | 207–210.5° BTDC, rising | F |
| 2.54 mm / 0.100 in | 26.5–30° ATDC | 190–193.5° BTDC | F |
| 3.81 mm / 0.150 in | 45–48.5° ATDC | 170.5–174.5° BTDC | F |
| 5.08 mm / 0.200 in | 71–74.5° ATDC | 144–148° BTDC | F |
| Peak | 104–107° ATDC | 107.5–110.5° BTDC | F |
| 5.08 mm / 0.200 in, falling | 136–141° ATDC | 70.5–73.5° BTDC | F |
| 3.81 mm / 0.150 in, falling | 162.5–167° ATDC | 44.5–47.5° BTDC | F |
| 2.54 mm / 0.100 in, falling | 180.5–185° ATDC | 26–29.5° BTDC | F |
| 1.27 mm / 0.050 in, falling | 197.5–201° ATDC | 9–12.5° BTDC | F |

Maximum valve lift measured at spring retainer, zero lash:

- intake: **≤0.245 in = 6.223 mm**, F
- exhaust: **≤0.255 in = 6.477 mm**, F

The exact S1 measured valve-lift-vs-crank-angle curve is **U**.

## 3.3 Port dimensions

| Quantity | Value | Unit | Grade | Exact source |
|---|---:|---|:---:|---|
| Intake port diameter at valve | 22.86–23.24 | mm | F | Older GX160 Technical Manual stock-port inspection limits |
| Exhaust port diameter at valve | 21.79–21.92 | mm | F | Same |
| Intake-port effective 1-D length | U | mm | U | Not found |
| Exhaust-port effective 1-D length | U | mm | U | Not found |

---

# 4. HARDWARE — C. Intake path

## 4.1 Carburettor

Older GX160 stock inspection source:
https://studylib.net/doc/18655427/honda-gx-160---tech-manual-updated-november-28--2012

| Quantity | Value | Unit | Grade |
|---|---:|---|:---:|
| Carburettor intake-end bore | ≤24.155 | mm | F |
| Throttle-end bore | ≤18.034 | mm | F |
| Venturi no-go diameter | 13.284 | mm | F |
| Exact S1 carburettor code | U | — | U |
| Exact S1 throttle-plate diameter | U | mm | U |
| Carburettor effective 1-D length | U | mm | U |
| Insulator/spacer length | U | mm | U |
| Insulator/spacer bore | U | mm | U |
| Air-cleaner type, S1 | U | — | U |
| Air-cleaner volume | U | cm³ | U |
| Measured air-cleaner restriction | U | Pa | U |

---

# 5. HARDWARE — C. Exhaust path and muffler

Ragan's tuned-pipe study selected lengths by LES and then physically manufactured and dynamometer-tested them.

Source: Ragan et al. 2015, §5 and Figure 5.
https://www.researchgate.net/publication/306407837_PARAMETERS_OPTIMIZATION_OF_MINIMOTOR

| Quantity | Value | Unit | Grade |
|---|---:|---|:---:|
| Tuned intake pipe length | 150 | mm | D |
| Tuned intake pipe diameter | U | mm | U |
| Tuned exhaust pipe length | 800 | mm | D |
| Tuned exhaust pipe diameter | U | mm | U |
| Stock exhaust-port effective length | U | mm | U |
| Stock muffler chamber volume(s) | U | cm³ | U |
| Baffle dimensions | U | mm | U |
| Perforate dimensions | U | mm | U |
| Tailpipe dimensions | U | mm | U |
| Measured valve/port flow coefficients | U | — | U |

## Exhaust back-pressure

**Measured back-pressure on the validation test configurations: U.**

Honda family installation requirement:

- tap position: **50 mm downstream of exhaust mounting flange**
- condition: **3060 rpm, rated continuous load**
- maximum allowable back-pressure: **11 kPa gauge**

Grade: **F**, because this is an installation/design limit, not a measured S1 stock-muffler value.

Source:
https://manualzz.com/doc/79522615/honda-gx120-160-200-owner-s-manual

Do **not** use 11 kPa as though the production muffler necessarily generates 11 kPa.

---

# 6. HARDWARE — D. Carburettor metering, ignition, mixture, fuel

## 6.1 Spark timing

S1 fixed ignition timing:

**25°CA BTDC**

Grade: **D**

Source: Ragan et al. 2015, conclusion item 1.
https://www.researchgate.net/publication/306407837_PARAMETERS_OPTIMIZATION_OF_MINIMOTOR

## 6.2 Full-load mixture

S1:

**2500 rpm, full load: λ = 0.95**

Grade: **D**

Source: Ragan et al. 2015, conclusion item 1.

A complete **λ/AFR-vs-speed full-load sweep for S1 is U**.

## 6.3 Carburettor metering

| Quantity | Value | Unit | Grade | Source |
|---|---:|---|:---:|---|
| Exact S1 main jet | U | — | U | Not found |
| Exact S1 main air bleed | U | — | U | Not found |
| Exact S1 emulsion geometry | U | — | U | Not found |
| Exact S1 float level | U | mm | U | Not found |
| Exact S1 choke state/configuration | U | — | U | Not found |
| GX160 family float height | 13.7 | mm | F | Honda service information |
| Racing/inspection main-jet maximum | 0.838 | mm | F | Older GX160 technical inspection material; not exact S1 calibration |
| Main air jet maximum | 1.491 | mm | F | Same |
| Pilot jet maximum | 0.343 | mm | F | Same |
| Pilot air jet maximum | 1.214 | mm | F | Same |

These F values must not be treated as exact S1 metering.

## 6.4 Fuel properties

### S1 Ragan

| Property | Value | Unit | Grade |
|---|---:|---|:---:|
| Octane | 95 | RON | D |
| Density | U | kg/m³ | U |
| LHV | U | MJ/kg | U |
| Detailed composition | U | — | U |

Ragan states RON95 gasoline for the compression-ratio experiments.

### S3 Kocakulak gasoline baseline

| Property | Value | Unit | Grade |
|---|---:|---|:---:|
| Density | 746 | kg/m³ | D |
| Octane | 95 | RON | D |
| Latent heat | 331.6 | kJ/kg | D |

Source: Kocakulak et al. 2023, fuel-property table.
https://dergipark.org.tr/tr/download/article-file/3285364

### S4 Liverpool laboratory fuel

| Property | Value | Unit | Grade |
|---|---:|---|:---:|
| Specific gravity | 0.738 | — | D for S4 |
| LHV | 44 | MJ/kg | D for S4 |

Source:
https://www.scribd.com/document/671702048/PET-script-MECH217-2021-22-DRAFT

---

# 7. HARDWARE — E. Temperatures

| Quantity | Result | Grade |
|---|---|:---:|
| S1 cylinder-head temperature | U | U |
| S1 oil temperature | U | U |

S2 measured ambient temperature for the airflow calculation, not head/oil temperature.

---

# 8. OUTPUTS — S1 Ragan exact Honda GX160

## 8.1 Main/full-load-type torque, power and BSFC curve

Ragan Figure 2 is captioned as the **main speed characteristics of the original Honda GX160 measured at dynamometer**.

Source:
https://www.researchgate.net/publication/306407837_PARAMETERS_OPTIMIZATION_OF_MINIMOTOR

### Digitisation method

- Figure: **Figure 2**
- sampling: **500-rpm intervals**
- approximate reading uncertainty: **±0.2 Nm torque, ±10 g/kWh BSFC**
- power below calculated from digitised torque using `P = T*n/9550`

| rpm | Torque | Power | BSFC | Grade |
|---:|---:|---:|---:|:---:|
| 1000 | ~9.4 Nm | 0.98 kW | ~344 g/kWh | R |
| 1500 | ~10.0 Nm | 1.57 kW | ~329 g/kWh | R |
| 2000 | ~9.85 Nm | 2.06 kW | ~324 g/kWh | R |
| 2500 | ~10.2 Nm | 2.67 kW | **316 g/kWh** | torque/power R; BSFC point D |
| 3000 | ~10.0 Nm | 3.14 kW | ~320 g/kWh | R |
| 3500 | ~9.3 Nm | 3.41 kW | ~335 g/kWh | R |
| 4000 | ~8.2 Nm | 3.43 kW | ~350 g/kWh | R |

The **316 g/kWh at 2500 rpm** point is stated directly in the paper.

## 8.2 Tuned-pipe configuration

Separate physical build: stock CR 8.5:1 plus 150-mm intake / 800-mm exhaust.

Reported over 2000–3000 rpm:

- torque increase: **+0.52 to +0.75 Nm**
- power increase: **+0.16 to +0.17 kW**
- BSFC reduction: **−2.8 to −4.1 g/kWh**
- relative BSFC reduction: **−0.9 to −1.3%**

Grade: **D**, reported experimental deltas.

A second modified build combined **CR 10:1 + tuned pipes** and produced:

- torque: **+0.95 to +1.93 Nm**
- power: **+0.2 to +0.5 kW**
- BSFC: **−13.06 to −18.36 g/kWh**

These modified-build outputs must not be merged into the stock S1 baseline.

## 8.3 Full-load airflow

**U**

Ragan Figure 6 gives LES-calculated volumetric efficiency, not measured airflow.

## 8.4 Cylinder pressure

Ragan Figure 3 contains measured pressure-vs-crank-angle traces for:

- CR 8.5:1
- CR 10:1
- CR 12:1

but the associated **engine speed and load are not adequately stated**.

Therefore:

- exact-Honda numeric pressure trace at stated speed/load: **U**
- CA10: **U**
- CA50: **U**
- CA90: **U**
- combustion duration: **U**

The pressure plot is not promoted to a validation trace without the missing operating condition.

---

# 9. OUTPUTS — S2 Aprinaldi exact Honda GX160

## 9.1 Test setup

The S2 thesis uses:

- brake dynamometer
- tachometer
- anemometer at the inlet of an air-stabilisation box
- volumetric fuel flask
- ambient-temperature measurement

The thesis directly measures air velocity and fuel-consumption time, then derives volume flow, air mass flow, fuel mass flow, AFR, torque, power and SFC.

The public thesis has an internal procedural inconsistency: the methodology refers to 3600 rpm before braking while the results describe a 4800-rpm initial condition. Therefore these points are retained as a **brake-load / non-confirmed-WOT dataset**, not promoted to full-load.

## 9.2 Raw standard-head measurements

Source: Aprinaldi 2014, Chapter IV, **Table 4.2**.

| rpm | Spring reading | Air velocity | Time for 4 mL fuel | Ambient |
|---:|---:|---:|---:|---:|
| 4623 | 0.20 kg | 0.8 m/s | 12.31 s | 29°C |
| 4472 | 0.23 kg | 0.7 m/s | 12.43 s | 29°C |
| 4252 | 0.28 kg | 0.7 m/s | 12.58 s | 29°C |
| 4023 | 0.41 kg | 0.6 m/s | 12.63 s | 29°C |
| 3851 | 0.50 kg | 0.5 m/s | 12.82 s | 29°C |

Grade: **D** for the directly read raw measurements.

## 9.3 Same-point calculated outputs

Source: Aprinaldi 2014, Chapter IV, **Table 4.9**.

| rpm | Qair | Air mass flow | Fuel mass flow | AFR | Torque | Power | SFC |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 4623 | 0.0019694 m³/s | 0.00236329 kg/s | 0.000243 kg/s | 9.730 | 0.3920 Nm | 0.2544 hp | 3.437 kg/(hp·h) |
| 4472 | 0.0017232 | 0.00206788 | 0.000241 | 8.597 | 0.4508 | 0.2830 | 3.060 |
| 4252 | 0.0017232 | 0.00206788 | 0.000238 | 8.701 | 0.5488 | 0.3275 | 2.612 |
| 4023 | 0.0014771 | 0.00177247 | 0.000237 | 7.488 | 0.8036 | 0.4538 | 1.878 |
| 3851 | 0.0012309 | 0.00147706 | 0.000233 | 6.333 | 0.9800 | 0.5297 | 1.5849 |

Grade: **R**, because these outputs are calculated from measured quantities.

### Example arithmetic

For 4623 rpm:

`A = Q/V = 0.0019694 / 0.8 = 0.00246175 m²`

Equivalent circular diameter:

`D_eq = sqrt(4A/π) = 0.05599 m = 56.0 mm`

Implied air density:

`rho_air = 0.00236329 / 0.0019694 ≈ 1.20 kg/m³`

Using the thesis gasoline density **747.45 kg/m³** and 4 mL fuel:

`m_dot_f = 747.45 * 4e-6 / 12.31 = 2.43e-4 kg/s`

`AFR = 0.00236329 / 0.000243 = 9.73`

This is the strongest recovered **same-physical-setup air + fuel + brake-load** dataset, but its WOT/full-load status is not established.

---

# 10. OUTPUTS — S3 Kocakulak explicit WOT Honda GX160

S3 explicitly operates the Honda GX160 at wide-open throttle at:

**2400, 2800, 3200, 3600, 4000 rpm**

Source:
https://dergipark.org.tr/tr/download/article-file/3285364

The authors note that the engine was previously used / partially worn.

### Digitised gasoline-baseline curves

Digitisation resolution: **published 400-rpm operating points**.

Approximate uncertainty:
- torque ±0.1 Nm
- SFC ±0.01 kg/kWh

| rpm | Torque | Power | SFC | Grade |
|---:|---:|---:|---:|:---:|
| 2400 | ~7.7 Nm | ~1.94 kW | ~0.46 kg/kWh | R |
| 2800 | ~8.4 Nm | ~2.46 kW | ~0.42 kg/kWh | R |
| 3200 | ~7.1 Nm | ~2.38 kW | ~0.46 kg/kWh | R |
| 3600 | ~6.7 Nm | ~2.53 kW | ~0.50 kg/kWh | R |
| 4000 | ~6.5 Nm | ~2.72 kW | ~0.58 kg/kWh | R |

Measured/published WOT airflow: **U**

Thus this study provides WOT torque/power/fuel behavior, but not the critical air-flow companion.

---

# 11. OUTPUTS — S4 University of Liverpool GX160 rig

The MECH217 GX160 laboratory script records at each throttle setting, including **fully open throttle**:

- engine speed
- DYNOmite brake torque
- fuel-flask emptying time
- hot-film MAF voltage

Source:
https://www.scribd.com/document/671702048/PET-script-MECH217-2021-22-DRAFT

This setup is explicitly capable of the required full-load same-test outputs:

- torque/power vs speed
- fuel flow / BSFC vs speed
- air flow vs speed

However:

**completed numerical test data = U**

Only the laboratory instructions and blank tables were recovered.

---

# 12. P1 — WEIMA 168FA cylinder-pressure fallback

This is **not an exact Honda GX160**.

Source:
https://yadda.icm.edu.pl/baztech/element/bwmeta1.element.baztech-388aa90b-607d-465e-b0e4-c76e90e119d0/c/Noga_Moskal_Evaluation_1_2024.pdf

The source identifies:

- engine: **WEIMA 168FA**
- displacement: **163 cm³**
- fixed ignition: **27° BTDC**
- pressure sensors: Optrand D822D6-SP and BorgWarner pressure-sensing glow plug
- speed range: **1500–3500 rpm**
- generator load: **10–25 A**

### Approximate pressure-trace digitisation

Figure 12, Optrand trace.

Resolution: mostly **60°CA**, with extra combustion points.

Approximate uncertainty: **±1 bar, ±10°CA**

| °CA | 2500 rpm / 25 A | 3000 rpm / 15 A |
|---:|---:|---:|
| 0 | ~1.4 bar | ~1.5 bar |
| 60 | ~1.2 | ~1.4 |
| 120 | ~1.0 | ~1.2 |
| 180 | ~1.1 | ~1.2 |
| 240 | ~1.4 | ~1.4 |
| 300 | ~2.7 | ~3.0 |
| 330 | ~5.0 | ~5.0 |
| 360 | ~12 | ~10 |
| 390 | ~30 | ~15–16 |
| 420 | ~18 | ~12 |
| 450 | ~8 | ~7 |
| 480 | ~4.2 | ~4 |
| 540 | ~2.2 | ~2.5 |
| 600 | ~1.5 | ~1.8 |
| 660 | ~1.2 | ~1.5 |
| 720 | ~1.2 | ~1.5 |

Grade: **R from F-family source**.

Do not merge this pressure trace with S1/S2/S3 Honda brake or airflow results.

---

# 13. Other requested outputs

| Output | Best result | Grade |
|---|---|:---:|
| Full-load torque/power vs speed | S1 main-speed characteristic; S3 explicit WOT | D/R |
| Full-load BSFC/fuel flow vs speed | S1 and S3 | D/R |
| **Full-load air flow vs speed** | **U** | U |
| Part-load/load-sweep air + fuel | S2 same physical setup | D raw / R derived |
| Exact-Honda cylinder pressure at stated speed/load | **U** | U |
| Burn angles CA10/50/90 | **U** | U |
| Motoring/friction/FMEP | **U** | U |
| Intake depression | **U** | U |
| Intake pressure trace | **U** | U |
| Exhaust pressure trace | **U** | U |
| Exhaust temperature in retained core datasets | **U** | U |

---

# 14. MODEL-DERIVED

The 2025 GT-Power GX160 study must not be treated as measured hardware.

Source:
https://www.combustion-engines.eu/pdf-200067-123531?filename=1D-simulation-of-the-impa.pdf

The following belong here:

- GT-Power valve discharge-coefficient curves
- calibrated AFR / volumetric-efficiency model inputs
- calibrated SITurb combustion multipliers
- friction multipliers/correlation parameters
- simulated air flow
- simulated combustion/burn-rate quantities

Example published calibration factors:

| rpm | Dilution multiplier | Flame-kernel multiplier | Turbulent-flame-speed multiplier | Taylor-length-scale multiplier |
|---:|---:|---:|---:|---:|
| 2000 | 0.292934 | 2.443412 | 0.693400 | 2.303240 |
| 2500 | 0.323000 | 2.760911 | 0.560654 | 1.988870 |
| 3000 | 0.331981 | 2.875282 | 0.412272 | 1.683686 |

These are **MODEL-DERIVED**, not physical-engine measurements.

The pressure-calibration source associated with this modelling chain is the **WEIMA 168FA** experiment, so its known-point cylinder pressure must remain F-family rather than D Honda data.

---

# 15. Same-physical-test-set matrix

| Setup | Torque / power | Fuel / BSFC | Air flow | Cylinder p(θ) | Mixture | Remarks |
|---|:---:|:---:|:---:|:---:|:---:|---|
| **S1 Ragan stock** | ✓ | ✓ | **U** | measured, but rpm/load U | λ=0.95 at 2500/full load | Main Honda brake reference |
| **S1 tuned pipes** | ✓ | ✓ | U | U | — | Separate 150/800-mm hardware build |
| **S1 CR modified** | ✓ | ✓ | U | measured but operating point U | RON95 | Separate piston/build |
| **S2 Aprinaldi standard head** | ✓ derived | ✓ derived | ✓ derived from measured velocity | U | ✓ derived AFR | Same-point coupled dataset; WOT not established |
| **S3 Kocakulak gasoline** | ✓ WOT | ✓ WOT | **U** | U | — | Used/worn engine |
| **S4 Liverpool** | instrumentation only | instrumentation only | instrumentation only | — | — | Completed numerical data not recovered |
| **P1 WEIMA** | separate generator load | — | U | ✓ | RON95 | Clone, F only |

---

# 16. Critical validation gaps

## Inputs

- exact measured S1 exhaust back-pressure at test points: **U**
- exact S1 full-load lambda/AFR speed sweep: **U** except λ=0.95 at 2500 rpm
- exact air-cleaner geometry/restriction: **U**
- complete intake segment geometry: **U**
- complete exhaust/muffler internal geometry: **U**
- exact S1 carburettor metering: **U**
- measured flow coefficients: **U**
- head/oil temperature: **U**

## Outputs

- **full-load air mass flow on the same setup as torque and fuel flow: U**
- exact-Honda cylinder-pressure trace at a fully stated speed/load: **U**
- burn angles: **U**
- motoring/friction/FMEP: **U**
- intake depression: **U**
- intake/exhaust pressure traces: **U**

The most important output limitation is that **no recovered numerical study contains confirmed full-load torque + fuel flow/BSFC + measured air flow together on one GX160 test setup**.

---

# 17. Downloadable/source-file register

| Source | Contents / relevance |
|---|---|
| https://www.researchgate.net/publication/306407837_PARAMETERS_OPTIMIZATION_OF_MINIMOTOR | Ragan et al. 2015: Honda GX160 brake curves, pressure Figure 3, CR variants, pipe optimisation, λ, spark timing |
| **User-uploaded Aprinaldi full thesis** | Methodology, apparatus, GX160 specification |
| **User-uploaded Aprinaldi Chapter IV PDF** | Tables 4.2–4.14: raw air velocity, fuel timing, airflow, fuel flow, AFR, torque, power, SFC |
| https://dergipark.org.tr/tr/download/article-file/3285364 | Kocakulak et al. 2023: exact GX160 WOT gasoline/MEK torque, power, SFC |
| https://manualzz.com/doc/62037288/honda-gx160--gx200-assembly-information | Honda assembly dimensions including valve heads |
| https://studylib.net/doc/18655427/honda-gx-160---tech-manual-updated-november-28--2012 | Older GX160 rod, cam timing/profile, valve lift, carb and port inspection dimensions |
| https://lightbournequipment.com/pdfs/GX120%2C%20GX160%2C%20GX200%20UT2%20SHOP%20MANUAL.pdf | Honda valve clearance/stem/guide/seat service information |
| https://manualzz.com/doc/79522615/honda-gx120-160-200-owner-s-manual | Honda exhaust back-pressure installation limit |
| https://yadda.icm.edu.pl/baztech/element/bwmeta1.element.baztech-388aa90b-607d-465e-b0e4-c76e90e119d0/c/Noga_Moskal_Evaluation_1_2024.pdf | WEIMA known-point cylinder pressure |
| https://www.combustion-engines.eu/pdf-200067-123531?filename=1D-simulation-of-the-impa.pdf | GT-Power MODEL-DERIVED data |
| https://www.scribd.com/document/671702048/PET-script-MECH217-2021-22-DRAFT | Liverpool GX160 full-load/part-load torque + fuel + MAF procedure; blank data tables |
| https://engines.honda.com/support-and-service/owners-manuals/gx160 | Official Honda GX160 owner-manual index |
| https://www.hondappsv.com/contents/menu/OTHERS/en/111/23/ | Honda GX160 dealer-service document index |

---

# 18. Overall verdict

This is a **useful but incomplete validation reference**.

Strong areas:

- baseline brake torque/power/BSFC
- older-family rod/cam/carb geometry sufficient to build a model
- exact fixed spark timing
- one exact full-load lambda point
- exact Honda part-load/load-sweep coupled air + fuel + torque dataset
- independent explicit-WOT Honda brake/fuel dataset

Weak / unresolved areas:

- full-load air flow on the same test as torque and fuel
- exact-Honda known-point cylinder pressure
- measured back-pressure
- detailed stock intake/exhaust geometry
- friction/motoring
- intake/exhaust dynamic pressure traces

No missing field above has been silently estimated.

---

# 19. JSON

The companion JSON file contains the same values in the requested structure:

`{name, value, unit, grade, source}`

See: **P2_Honda_GX160_validation_pack.json**
