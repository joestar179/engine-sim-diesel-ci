# Diesel Validation Engine Reference Pack
## Cummins 4B3.9-G1 / 4BT3.9-G1 and John Deere 4045DF150 / 4045TF250
Research freeze: 30 September 2026

### Purpose
This pack defines two controlled naturally-aspirated/turbocharged diesel pairs for validating the Engine Simulator diesel and forced-induction implementation. The intent is to validate physics across two unrelated engine families without changing known physical parameters merely to force target power, torque, boost, transient response or sound.

### Evidence classes
- **D — Documented, exact configuration:** directly tied to the locked engine/model/option/CPL.
- **F — Documented family/compatible hardware:** real manufacturer/family data, but not yet proven to be the exact locked configuration.
- **R — Derived:** calculated only from documented inputs; assumptions are stated.
- **C — Calibration/estimated:** not documented and must be tuned only within physically plausible bounds.
- **U — Unresolved:** no defensible value found yet. Do not silently substitute a neighboring engine/turbo value.

---

# 1. Configuration locks

## Cummins pair — primary low-speed/generator validation
### Naturally aspirated
- **Engine:** Cummins/Dongfeng Cummins 4B3.9-G1
- **Curve/data sheet:** FR92340
- **Configuration:** D381004GX02
- **CPL:** 3114
- **Revision:** 00, 15 April 2009
- **Rated condition used here:** 1500 rpm generator service
- **Fuel/governor:** BYC A pump / RSV mechanical governor
- **Status:** D

### Turbocharged
- **Engine:** Cummins/Dongfeng Cummins 4BT3.9-G1
- **Curve/data sheet:** FR92341
- **Configuration:** D382012GX02
- **CPL:** 3115
- **Revision:** 00, 15 April 2009
- **Rated condition used here:** 1500 rpm generator service
- **Fuel/governor:** BYC A pump / RSV mechanical governor
- **Status:** D

**Important version warning:** do not merge this 2009 mechanical 4BT3.9-G1 dataset with later 4BT3.9-G2/CPL 3115 sheets. Later G2 revisions use different governor/control details and, in some revisions, 18.0:1 compression. CPL equality by itself is not sufficient evidence that every parameter is interchangeable.

## John Deere pair — independent variable-speed validation
### Naturally aspirated
- **Engine:** John Deere PowerTech 4045DF150
- **Locked injection-pump option:** 1601
- **Original pump:** RE61649; replacement RE67557
- **Rated power/speed:** 60 kW intermittent at 2500 rpm
- **Slow/fast idle:** 850 / 2700 rpm
- **Governor:** standard mechanical, 7–10% family specification
- **Status:** D

### Turbocharged
- **Engine:** John Deere PowerTech 4045TF250
- **Locked injection-pump option:** 1606
- **Original pump:** RE64133; replacement RE505927
- **Rated power/speed:** 93 kW intermittent at 2400 rpm
- **Slow/fast idle:** 850 / 2600 rpm
- **Governor:** standard mechanical
- **Status:** D

---

# 2. Cummins exact A/B dataset

| Parameter | 4B3.9-G1 FR92340 / CPL3114 | 4BT3.9-G1 FR92341 / CPL3115 | Class |
|---|---:|---:|---|
| Cylinders | 4 inline | 4 inline | D |
| Bore | 102 mm | 102 mm | D |
| Stroke | 120 mm | 120 mm | D |
| Displacement | 3.9 L | 3.9 L | D |
| Compression ratio | 17.3:1 | 17.3:1 | D |
| Aspiration | Naturally aspirated | Turbocharged | D |
| Fuel system | BYC A direct injection | BYC A direct injection | D |
| Governor | RSV mechanical | RSV mechanical | D |
| Governor regulation | ≤8% | ≤8% | D |
| Rated test speed | 1500 rpm | 1500 rpm | D |
| Prime power | 24 kW | 36 kW | D |
| Standby power | 27 kW | 40 kW | D |
| Prime BSFC | 245 g/kWh | 229 g/kWh | D |
| Standby BSFC | 244 g/kWh | 228 g/kWh | D |
| Prime fuel consumption | 7.1 L/h | 10.0 L/h | D |
| Standby fuel consumption | 8.0 L/h | 11.1 L/h | D |
| Idle range | 950–1050 rpm | 800–1000 rpm | D |
| Friction power @1500 rpm | 8.2 kW | 8.2 kW | D |
| Prime intake airflow | 33 L/s | 44 L/s | D |
| Standby intake airflow | 33 L/s | 45 L/s | D |
| Prime exhaust gas temperature | 380 °C | 463 °C | D |
| Standby exhaust gas temperature | 410 °C | 487 °C | D |
| Prime exhaust volume flow | 68 L/s | 101 L/s | D |
| Standby exhaust volume flow | 71 L/s | 108 L/s | D |
| Wet engine weight | 308 kg | 321 kg | D |
| Rotating-component inertia, no flywheel | 0.143 kg·m² | 0.143 kg·m² | D |
| Complete-engine roll MOI | 16.5 kg·m² | 16.5 kg·m² | D |
| Complete-engine pitch MOI | 41.1 kg·m²* | 41.1 kg·m² | D |
| Complete-engine yaw MOI | 35.4 kg·m²* | 35.4 kg·m² | D |
| Maximum exhaust backpressure | 10 kPa | 10 kPa | D |
| Nominal acceptable exhaust pipe | 75 mm | 75 mm | D |
| Clean air-filter restriction limit | 4 kPa | 4 kPa | D |
| Dirty air-filter restriction limit | 6 kPa | 6 kPa | D |
| Intake pipe | 76 mm | 76 mm | D |
| Engine-only coolant capacity | 7.2 L | 7.2 L | D |
| Cranking system | 12 or 24 V | 12 or 24 V | D |
| Minimum battery capacity at -12 °C | 625 CCA (12 V) / 312 CCA (24 V) | same | D |

\*The NA sheet's reproduced layout is less clean than the turbo sheet; the complete-engine inertia values are treated as documented because the paired sheet and mirrors preserve the same engine-assembly values, but the no-flywheel crank rotating inertia (0.143 kg·m²) is the more important simulation anchor.

## Cummins part-load fuel data
### 4B3.9-G1, prime rating at 1500 rpm
| Load | Power | BSFC | Fuel volume |
|---:|---:|---:|---:|
| 100% | 24 kW | 245 g/kWh | 7.1 L/h |
| 75% | 18 kW | 260 g/kWh | 5.7 L/h |
| 50% | 12 kW | 296 g/kWh | 4.3 L/h |
| 25% | 6 kW | 390 g/kWh | 2.8 L/h |

### 4BT3.9-G1, prime rating at 1500 rpm
| Load | Power | BSFC | Fuel volume |
|---:|---:|---:|---:|
| 100% | 36 kW | 229 g/kWh | 10.0 L/h |
| 75% | 27 kW | 241 g/kWh | 7.9 L/h |
| 50% | 18 kW | 270 g/kWh | 5.9 L/h |
| 25% | 9 kW | 363 g/kWh | 4.0 L/h |

## Cummins derived validation targets
The sheets use correction conditions of 100 kPa total barometric pressure, 25 °C inlet air and 1 kPa water-vapour pressure.

- **Prime power increase:** 24 → 36 kW = **+50.0%**. [R]
- **Prime torque at 1500 rpm:** approximately **152.8 N·m NA** and **229.2 N·m turbo**. [R]
- **Prime published intake-volume-flow increase:** 33 → 44 L/s = **+33.3%**. [R]
- **Prime exhaust-volume-flow increase:** 68 → 101 L/s = **+48.5%**. [R]
- Using moist-air density calculated from the published correction conditions gives approximately **1.164 kg/m³**. Applying this to the published intake flows yields approximately **0.0384 kg/s NA** and **0.0512 kg/s turbo**. [R]
- Prime fuel mass from BSFC is **5.88 kg/h NA** and **8.244 kg/h turbo**. [R]
- Corresponding rough air/fuel ratios from those two published datasets are approximately **23.5:1 NA** and **22.4:1 turbo**. [R]
- These derived AFR values are validation checks, not exact trapped-cylinder AFR: the published volumetric-airflow reference basis must be confirmed before using them as combustion inputs.

### Cummins fields still unresolved
1. **Exact turbocharger make/model/assembly number for FR92341/CPL3115** — U.
2. **Exact compressor inducer/exducer dimensions for that CPL3115 turbo** — U.
3. **Exact turbine wheel dimensions for that CPL3115 turbo** — U.
4. **Turbine housing A/R or effective nozzle/throat area** — U.
5. **Compressor map / turbine map** — U.
6. **Turbo rotor mass and polar moment of inertia** — U.
7. **Turbo maximum shaft speed** — U.
8. **Published boost/manifold pressure for exact FR92341/CPL3115** — U.
9. **Exact injection timing for the BYC-A/RSV CPL3114 and CPL3115 pair** — U.
10. **Injection rate shape / injection duration / pump delivery versus rack and rpm** — U.
11. **Exact injector opening pressure, hole count, hole diameter and spray angle for these two CPLs** — U.
12. **Valve timing events and cam-lobe geometry** — U.
13. **Connecting-rod centre-to-centre length** — U.
14. **Flywheel inertia for the selected generator configuration** — U.
15. **Starter torque-speed curve** — U.

### Cummins family-level turbo proxy: do not promote to exact CPL3115 data
Public Cummins parts data confirms CPL3115 as a turbocharged 4BT3.9 generator configuration with BYC-A pump, dry exhaust manifold and edge-filter injectors, but the public option result does **not** expose the turbocharger assembly part number. Therefore older 4BT/Holset H1C dimensions found elsewhere must remain family-level comparison data only. They are useful for sanity checking but are **not admissible as exact FR92341 turbo geometry**.

---

# 3. John Deere exact A/B dataset

## Core geometry and configuration
| Parameter | 4045DF150 | 4045TF250 | Class |
|---|---:|---:|---|
| Cylinders | 4 inline | 4 inline | D |
| Cycle | Four-stroke | Four-stroke | D |
| Bore | 106 mm | 106 mm | D |
| Stroke | 127 mm | 127 mm | D |
| Displacement | 4.5 L | 4.5 L | D |
| Compression ratio | 17.6:1 | 17.0:1 | D |
| Valves/cylinder | 1 intake / 1 exhaust | 1 intake / 1 exhaust | D |
| Firing order | 1-3-4-2 | 1-3-4-2 | D |
| Combustion | Direct injection | Direct injection | D |
| Aspiration | Naturally aspirated | Turbocharged | D |
| Locked pump option | 1601 | 1606 | D |
| Original injection pump | RE61649 | RE64133 | D |
| Replacement pump | RE67557 | RE505927 | D |
| Rated speed | 2500 rpm | 2400 rpm | D |
| Slow idle | 850 rpm | 850 rpm | D |
| No-load fast idle | 2700 rpm | 2600 rpm | D |
| Mechanical-governor family regulation | 7–10% | 7–10% | D/F |
| Dynamic timing, original pump | 8.0° BTDC | 4.5° BTDC | D |
| Dynamic timing, replacement pump | 8.5° BTDC (RE67557) | 4.5° BTDC remains listed for 1606 family | D |
| Continuous rated power | 54 kW @2500 | 84 kW @2400 | D |
| Intermittent rated power | 60 kW @2500 | 93 kW @2400 | D |
| Friction power at rated speed | 22 kW | 21 kW | D |
| Dry power-unit weight incl. flywheel housing/flywheel/electrics | 497 kg | 548 kg | D |

## John Deere variable-speed performance curves
### 4045DF150
| rpm | Continuous kW | Intermittent kW | Intermittent torque N·m | BSFC g/kWh |
|---:|---:|---:|---:|---:|
| 2500 | 54 | 60 | 228 | 237 |
| 2400 | 53 | 59 | 235 | 233 |
| 2200 | 51 | 57 | 248 | 227 |
| 2000 | 49 | 54 | 260 | 222 |
| 1800 | 46 | 51 | 270 | 219 |
| 1600 | 42 | 47 | 278 | 218 |
| 1400 | 38 | 42 | 286 | 219 |
| 1200 | 33 | 37 | 292 | 221 |
| 1000 | — | 30 | 290 | 225 |

Additional documented rated-point data:
- Peak torque: 263 N·m continuous / 292 N·m intermittent at 1200 rpm.
- BMEP: 573 / 636 kPa.
- Air:fuel ratio: 25:1 continuous / 22:1 intermittent.
- Noise at 1 m: 97.0 / 97.5 dB(A).
- Smoke: <2 Bosch number.
- Altitude capability: 600 m.

### 4045TF250
| rpm | Continuous kW | Intermittent kW | Intermittent torque N·m | BSFC g/kWh |
|---:|---:|---:|---:|---:|
| 2400 | 84 | 93 | 371 | 224 |
| 2200 | 82 | 91 | 396 | 220 |
| 2000 | 78 | 87 | 415 | 218 |
| 1800 | 73 | 81 | 428 | 217 |
| 1600 | 67 | 74 | 440 | 217 |
| 1400 | 59 | 65 | 445 | 220 |
| 1200 | — | 55 | 434 | 229 |
| 1000 | — | 39 | 375 | 239 |

Additional documented rated-point data:
- Peak torque: 401 N·m continuous / 445 N·m intermittent at 1400 rpm.
- BMEP: 930 / 1030 kPa.
- Air:fuel ratio: 29.4:1 continuous / 28.4:1 intermittent.
- Noise at 1 m: 95.5 / 96.5 dB(A).
- Smoke: <2 Bosch number.
- Altitude capability: 2300 m.

## Air, exhaust and fuel-system data
| Parameter | 4045DF150 | 4045TF250 | Class |
|---|---:|---:|---|
| Rated engine airflow, continuous | 4.7 m³/min | 8.2 m³/min | D |
| Rated engine airflow, intermittent | 4.7 m³/min | 8.5 m³/min | D |
| Exhaust flow, continuous | 12.4 m³/min | 19.7 m³/min | D |
| Exhaust flow, intermittent | 13.1 m³/min | 21.0 m³/min | D |
| Exhaust temperature, continuous | 536 °C | 476 °C | D |
| Exhaust temperature, intermittent | 582 °C | 495 °C | D |
| Max exhaust backpressure | 7.5 kPa | 7.5 kPa | D |
| Recommended exhaust pipe | 63.5 mm | 102 mm | D |
| Rated fuel consumption, continuous | 13.0 kg/h | 18.9 kg/h | D |
| Rated fuel consumption, intermittent | 14.3 kg/h | 20.9 kg/h | D |
| Injection-pump maker on period power-unit sheet | Stanadyne | Lucas | D |
| Current/replacement pump identity | RE67557 family; Stanadyne rotary architecture documented in cross-reference material | RE505927; later cross-reference identifies Delphi DP200 lineage | F/D part mapping |

## Injection system
John Deere's CTM207 mechanical-fuel manual provides unusually strong reference data for this family.

### Injection timing
- **4045DF150 option 1601, original RE61649:** **8.0° BTDC**. [D]
- **4045DF150 option 1601, replacement RE67557:** **8.5° BTDC**. [D]
- **4045TF250 option 1606, RE64133 → RE505927:** **4.5° BTDC**. [D]

### Nozzle data
The Deere mechanical-fuel manual specifies:
- Naturally aspirated **RE60062** nozzles: setting pressure **238–244 bar**; minimum check pressure 218 bar; used-nozzle minimum 198 bar. [F, exact option mapping independently supported for 4045DF150/1601 by Deere replacement guide]
- Turbocharged **RE48786** nozzles: setting pressure **255–260 bar**; minimum check pressure 246 bar; used-nozzle minimum 235 bar. [F]
- Maximum opening-pressure difference cylinder-to-cylinder: **7 bar**. [F]
- **4 orifices per nozzle**. [F]
- NA nozzle tip orifice ID: **0.27 mm**. [F]
- Turbo nozzle tip orifice ID: **0.29 mm**. [F]
- Turbo-family nozzle spray angle: **144°**. [F]

Deere's replacement-parts guide explicitly maps RE60062-family nozzles to 4045DF150 option **1601**. The current 4045TF250 guide maps the 1606 fuel pump but does not, in the same public page, tie RE48786 directly to option 1606; therefore the turbo nozzle dimensions/opening pressure remain **family-documented** rather than promoted to exact-option status.

## Turbo boost — exact option 1606
The Deere technical manual lists for:
- **4045TF250**
- **pump option 1606**
- **RE64133 / replacement RE505927**
- **93 kW at 2400 rpm**

a full-load rated-speed turbo boost range of:

**109–133 kPa (1.1–1.3 bar / 16–19 psi).** [D]

At the power-curve reference barometer of 99 kPa, treating this as gauge boost gives:
- manifold absolute pressure ≈ **208–232 kPa** [R]
- manifold-to-ambient pressure ratio ≈ **2.10–2.34** [R]

This is a manifold/boost validation target, not automatically the compressor-outlet pressure ratio; intake/filter/compressor-discharge/manifold pressure losses must remain separate in the simulator.

## John Deere NA→turbo comparisons
At the common 2400-rpm point:
- continuous power: **53 → 84 kW = +58.5%** [R]
- intermittent power: **59 → 93 kW = +57.6%** [R]

The turbo engine simultaneously shows far greater airflow and lower full-load exhaust temperature than the NA engine at their respective published rated conditions. This is a valuable coupled target for validating air mass, fueling and combustion efficiency instead of matching horsepower alone.

---

# 4. John Deere turbocharger hardware: documented compatible hardware, exact option mapping not yet locked

A real BorgWarner/Schwitzer **S2A / S2A090**, assembly **318615**, is documented for John Deere 4045/4045T industrial and genset applications. Public cross-reference listings connect it to Deere numbers including RE508971 and related assemblies.

Documented component data for **318615**:
- CHRA: **318613**
- bearing housing: **318609**
- compressor cover: **317367**
- compressor wheel: **197611 / 409096-0008**
- compressor-wheel published diameters: **38.1 mm and 60.95 mm**
- compressor blades: **6+6**
- turbine wheel: **314653**
- turbine-wheel published diameters: **52.8 mm and 61.0 mm**
- turbine blades: **10**
- turbine housing: **318567**
- backplate: **197340**

**Critical caveat:** public evidence found so far establishes 318615/S2A as compatible 4045/4045T hardware, but it does **not yet prove that 4045TF250 option 1606 used this exact turbo assembly.** Therefore these wheel dimensions are **F**, not **D**.

Also preserve the raw vendor diameter labels rather than silently interpreting “inducer/exducer”; aftermarket catalog terminology for turbine-wheel diameters can be inconsistent.

### Deere turbo fields still unresolved for option 1606
1. Exact turbo assembly / turbo option code fitted to the locked 1606 build — U.
2. Exact compressor map — U.
3. Exact turbine flow map — U.
4. Exact turbine housing A/R / effective nozzle throat area — U.
5. Exact turbo rotor mass — U.
6. Exact turbo polar inertia — U.
7. Exact maximum turbo shaft speed — U.
8. Compressor/turbine efficiency curves — U.
9. Exact compressor-discharge temperature at rated load — U.
10. Whether the compatible 318615/S2A geometry applies to the locked 1606 engine — U until a Deere build sheet/parts page connects the two.

---

# 5. Starting and rotational-dynamics data

## Cummins
Useful exact anchors:
- Crank/engine rotating components excluding flywheel: **0.143 kg·m²** for both NA and turbo engines.
- 12 V or 24 V heavy-duty positive-engagement starter.
- Minimum recommended battery capacity at -12 °C: **625 CCA (12 V)** or **312 CCA (24 V)**.
- Exact starter torque-speed curve and selected flywheel inertia remain unresolved.

This makes Cummins the stronger pair for validating crank acceleration/deceleration because a real engine rotating-inertia value is available.

## Deere
Useful exact/family anchors:
- Recommended battery: **640 CCA at 12 V / 570 CCA at 24 V**.
- Starter rolling current: approximately **780 A (12 V) / 600 A (24 V) at 0 °C** and **1000 A / 700 A at -30 °C**.
- Deere parts guides list several starter options, including 2.5–4.2 kW units depending on option code.
- The exact starter option for the locked performance-sheet build is not identified here.
- Crankshaft/flywheel moment of inertia is not published in the sources found.

Therefore Deere transient inertia remains partly calibration-driven unless a flywheel drawing or inertia test is acquired.

---

# 6. Procedural-audio reference material

These recordings are useful only as **qualitative references** unless the exact engine/CPL, microphone geometry, load, room/vehicle acoustics and recording chain are known.

## Cummins 4BT
- “4BT Cummins Startup” — rebuilt 4BT startup.
  https://www.youtube.com/watch?v=K2I8929Z3HI
- “4bt Cummins turbo diesel engine start up and run” — VE-pumped 4BT hanging from an engine hoist, useful because vehicle structure masks relatively little of the engine sound.
  https://www.youtube.com/watch?v=aPqxe39-hFg
- Seaboard Marine 4BT engine-test/start-up gallery:
  https://www.sbmar.com/video-gallery-engine-startups-4bt/

Use for:
- cranking cadence;
- first-firing transition;
- four-cylinder combustion pulse character;
- idle irregularity/governor behaviour;
- turbo/airflow spectral component under load.

Do **not** tune exact CPL3115 combustion or turbo parameters to make these unrelated 4BT recordings match.

## John Deere
A commercial listing for an actual **4045DF150 naturally aspirated running engine** provides a linked run video:
- listing: https://www.ngbequipment.com/product-page/engine-john-deere-4045-1
- linked video: https://www.youtube.com/watch?v=M6zrO00ZIVQ

A clean exact-option 4045TF250/1606 loaded recording was not found in this pass. Generic 4045 turbo recordings exist, but should remain qualitative.

---

# 7. Recommended validation sequence

## Stage A — Cummins pair
1. Build **4B3.9-G1 FR92340/CPL3114** with turbo system disabled.
2. Match documented 1500-rpm prime and part-load fuel/power points without altering bore, stroke, compression, inertia or published airflow.
3. Validate starter → first firing → governor capture → 1500-rpm governed operation.
4. Add **4BT3.9-G1 FR92341/CPL3115** using the same generic diesel core.
5. Before exact turbo hardware is known, use a clearly labelled **calibration turbo** constrained by:
   - 44–45 L/s published intake flow;
   - 36/40 kW prime/standby power;
   - 463/487 °C EGT;
   - 101/108 L/s exhaust flow;
   - exact crank rotating inertia 0.143 kg·m²;
   - documented fuel rates.
6. Do **not** claim turbo geometry validation until CPL3115 hardware is identified.

## Stage B — Deere pair
1. Build **4045DF150 option1601** using the full published variable-speed torque/BSFC curve.
2. Build **4045TF250 option1606** with documented 4.5° BTDC timing and full-load boost target 109–133 kPa gauge.
3. Validate the entire 1000–2400/2500 rpm curves, not just rated horsepower.
4. Check the coupled targets:
   - torque;
   - BSFC/fuel mass;
   - air flow;
   - AFR;
   - exhaust flow;
   - EGT;
   - boost.
5. If the same generic turbo model can reproduce the Deere behaviour after being developed on the Cummins without engine-specific C++ changes, that is strong evidence that the abstraction generalises.

---

# 8. What must remain calibration values for now

The following should remain explicit calibration inputs unless/until a configuration-specific source is acquired:
- discharge coefficients / effective intake and exhaust valve flow;
- exact cam events/lift curves;
- diesel ignition-delay coefficients;
- premixed/diffusion burn split;
- Wiebe/heat-release-shape parameters if used;
- injection rate shape where pump delivery curves are unavailable;
- heat-transfer correction factors;
- accessory load beyond documented friction;
- turbo map shape where the exact map is unavailable;
- turbo rotor inertia where the exact rotating assembly is unavailable;
- turbine effective throat/A/R where not documented;
- low-speed mechanical-friction extrapolation below published test speeds;
- procedural-audio filter/gain coefficients.

These may be calibrated **against multiple documented outputs simultaneously**, but must never be relabelled as manufacturer specifications.

---

# 9. Highest-value remaining acquisition targets

### Cummins
1. Cummins QuickServe/parts build list for **CPL3115 / configuration D382012GX02** showing the exact turbo option and assembly number.
2. BYC-A pump calibration sheet for **CPL3114 and CPL3115**.
3. Injector option/part numbers and nozzle specification for the two CPLs.
4. Exact generator flywheel option and inertia/drawing.
5. Turbo OEM map or service drawing once the assembly number is known.

### John Deere
1. Parts/build sheet connecting **4045TF250 option1606** to its exact turbocharger option code/part number.
2. If S2A/318615 is confirmed, BorgWarner/Schwitzer compressor/turbine maps and housing flow geometry.
3. Rotor component weights/inertia.
4. Flywheel option drawing or inertia.
5. Exact option1606 injector-part mapping if a stronger primary parts source can be found.
6. Controlled 4045TF250/1606 audio/video at known speed and load.

---

# 10. Source register

## Cummins exact engine sheets
1. **Dongfeng Cummins 4B3.9-G1 FR92340, CPL3114, Rev 00 15APR2009** — direct PDF mirror.
   https://www.baifapower.com/static/upload/download/FADONGJI/4B3.9-G1.pdf
2. Alternate FR92340 PDF mirror:
   https://www.hosempower.com/uploadfile/downloads/FR92340%204B3.9-G1%20datasheet_00%20En.pdf
3. **Dongfeng Cummins 4BT3.9-G1 FR92341, CPL3115, Rev 00 15APR2009** — direct PDF mirror.
   https://tech-expo.ru/upload/iblock/2c6/4BT3.9_G1.pdf
4. Alternate FR92341 mirror:
   https://fdkenergy.com/wp-content/uploads/2021/08/CD40-H1-4BT3.9G1.pdf
5. Cummins option-detail reproduction for **PP43115-04 / CPL3115**; confirms 4BT3.9 generator, turbocharged, BYC-A pump, dry exhaust manifold and edge-filter injectors, but does not expose the turbo assembly:
   https://www.scribd.com/document/677265794/02-Performance-Parts

## John Deere engine/performance sources
6. **John Deere 4045DF150 official generator-drive specification**, Deere-hosted PDF:
   https://www.deere.com/assets/pdfs/common/industries/engines-and-drivetrain/specsheets/4045DF150_J.pdf
7. **4045DF150 variable-speed PowerTech performance sheet** (period Deere sheet mirror):
   https://www.stuartgroup.co.uk/media/pumps/products/2455/files/4045DF150-engine.pdf
8. **4045TF250 variable-speed PowerTech performance sheet**:
   https://www.ramamotori.it/wp-content/uploads/2024/02/4045TF250.pdf
9. John Deere 4045DF150 current replacement-parts guide:
   https://www.deere.com/assets/pdfs/common/qrg/engine-qrg/4045df150.pdf
10. John Deere 4045TF250 current replacement-parts guide:
    https://www.deere.com/assets/pdfs/common/qrg/4045tf250.pdf

## John Deere technical manuals
11. **John Deere CTM207 — PowerTech 4.5 L & 6.8 L Mechanical Fuel Systems**; mirror hosted by Doosan dealer support. Contains injection timing, nozzle opening pressure, hole count/diameter and related diagnostic specifications.
    https://dealers.doosanportablepower.com/emea/documents/Service/01_Portable%20Compressors/Engine%20Manuals/07_7-71%2C12-56_4IRD5N%20JD/CTM207%20JD_4045_6068_Mechanical_Fuel.pdf
12. **John Deere CTM104 — PowerTech 4.5 L & 6.8 L Base Engine**; mirror hosted by Doosan dealer support. Contains the option-specific turbo boost table including 4045TF250 option1606.
    https://dealers.doosanportablepower.com/emea/documents/Service/01_Portable%20Compressors/Engine%20Manuals/09_7-170%2C10-125%2C14-115_6IRF8TE%20JD/CTM104%20_30JUN05_%20PWT4.5and6.8Base_GB.pdf
13. John Deere 4045/6068 operation manual mirror; useful for engine/pump option/rated-speed cross-check:
    https://www.marinedieselbasics.com/wp-content/uploads/edd/JDeere-4045-Operation-1996.pdf

## Deere-compatible turbo hardware
14. BorgWarner/Schwitzer **318615 S2A/S2A090** detailed component listing for Deere 4045/4045T:
    https://turboturbos.com/products/318615

## Audio/reference recordings
15. Cummins 4BT startup after rebuild:
    https://www.youtube.com/watch?v=K2I8929Z3HI
16. VE-pumped Cummins 4BT standalone start/run:
    https://www.youtube.com/watch?v=aPqxe39-hFg
17. Seaboard Marine 4BT engine-test gallery:
    https://www.sbmar.com/video-gallery-engine-startups-4bt/
18. John Deere 4045DF150 running-engine listing/video:
    https://www.ngbequipment.com/product-page/engine-john-deere-4045-1

---

# 11. Current decision

**Cummins 4B3.9-G1 / 4BT3.9-G1 remains the best first implementation pair** because it gives a very clean 1500-rpm NA/turbo comparison and, unusually, an exact published crank/engine rotating inertia.

**John Deere 4045DF150 / 4045TF250 is the stronger full-range physics validation pair** because it supplies variable-speed power, torque, BSFC, airflow, AFR, exhaust flow/temperature, injection timing and an exact full-load turbo-boost range for option1606.

The two should be used together:
- Cummins to validate starting, governor capture, generator-speed behavior and rotational dynamics.
- Deere to validate the generic combustion/airflow/turbo model across a broad speed/load envelope.

The ALCO 251 should remain the eventual large-engine validation/end-use case, not the engine used to invent the generic diesel/turbo physics.
