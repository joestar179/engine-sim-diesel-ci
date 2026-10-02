# P5 — Volvo D13 heavy-duty diesel validation pack

## 1. Configuration control

**Chosen base engine:** Volvo **D13 US 2010**, 12.8 L inline-six, 131 × 158 mm, 16.0:1, 373 kW / 500 hp, 2373 Nm / 1750 lb-ft.

This is the engine explicitly identified in Rijpkema et al. 2022, Table 2. It is used as the base because it provides the richest accessible exact-configuration experimental dataset. Pecoraro 2013 remains a closely related D13 GT-Power validation study, but its repository record states that validation used a **similar engine** and the thesis bitstream is not publicly exposed. Therefore Pecoraro-derived quantities are not silently promoted to D-grade base-engine numbers.

**Grades**
- **D** — exact selected D13 US 2010 / 500 hp configuration, or direct measurements on it.
- **F** — D13 family / closely related D13F, D13H, D13-700 or Mack MP8 hardware.
- **R** — derived; arithmetic shown.
- **U** — not found numerically.
- `N/A` is used only where a requested quantity does not exist for this architecture (e.g. common-rail pressure on the US10 unit-injector system).

**Importance**
- **[E]** essential
- **[I]** important
- **[N]** nice to have

---

## 2. Primary engine specification

| Item | Value | Grade | Source |
|---|---:|:---:|---|
| Engine | Volvo D13 US 2010 | D | Rijpkema et al. 2022, p.3, Table 2 — https://research.chalmers.se/publication/525850/file/525850_Fulltext.pdf |
| Configuration | 4-stroke inline 6, EGR | D | Same |
| Bore | 131 mm | D | Same |
| Stroke | 158 mm | D | Same |
| Nominal displacement | 12.8 L | D | Same |
| Compression ratio | 16.0:1 | D | Same |
| Peak power | 373 kW / 500 hp | D | Same |
| Peak torque | 2373 Nm / 1750 lb-ft | D | Same; same rating also in Volvo D13-500/1750 specification |
| Aspiration | Turbocharged | D | Rijpkema et al. 2022, Table 2 |
| Charge-air cooling | Present | D | Rijpkema et al. 2022, Sec. 2 / Fig. 2 |
| EGR | Present | D | Rijpkema et al. 2022, Table 2 |
| EGR architecture | Long-route | D | Rijpkema PhD, p.25, Table 3.1 — https://research.chalmers.se/publication/523780/file/523780_Fulltext.pdf |

### Derived geometry

Using the published bore and stroke:

`Vs,cyl = π/4 × 0.131² × 0.158 = 0.00212956 m³ = 2.12956 L`

`Vs,total = 6 × 2.12956 = 12.77735 L`

Using CR = 16.0:

`Vc,cyl = Vs,cyl / (CR - 1) = 2.12956 / 15 = 0.141971 L = 141.97 cm³`

At the published peak torque:

`BMEP = 4πT/Vd = 4π×2373 / 0.0128 = 2.3297 MPa = 23.30 bar`

| Derived item | Value | Grade |
|---|---:|:---:|
| Swept volume / cylinder | 2.12956 L | R |
| Geometric six-cylinder displacement | 12.77735 L | R |
| Clearance volume / cylinder | 141.97 cm³ | R |
| Peak-torque BMEP | 23.30 bar | R |

---

# INPUTS

## A. Cranktrain / combustion chamber

| Importance | Parameter | Value | Grade | Source / note |
|---|---|---:|:---:|---|
| [E] | Connecting-rod length | **259.0 mm** | F | Volvo Penta D13 family workshop specification, “Connecting rod, length (E) 259.0 mm” — https://www.scribd.com/document/938147474/Groups-20-26. Not independently tied to US10 truck test engine. |
| [E] | Compression ratio | **16.0:1** | D | Rijpkema et al. 2022, p.3, Table 2 — https://research.chalmers.se/publication/525850/file/525850_Fulltext.pdf |
| [I] | Firing order | **1-5-3-6-2-4** | F | Volvo Penta D13 family workshop specification; family architecture. |
| [N] | Piston bowl diameter/depth/volume | **U** | U | No configuration-specific bowl geometry recovered. |

**Recommended simulator use:** 259 mm is a defensible family fallback. Rod/stroke ratio = `259/158 = 1.639` (R). Keep an F flag in the model input database.

## B. Valves / engine brake / VVA

| Importance | Parameter | Value | Grade | Source / note |
|---|---|---:|:---:|---|
| [I] | Intake valve head diameter | **42 mm** | F | Volvo D13F/H family workshop specification — https://studylib.net/doc/27751421/volvo-d13f-engine-workshop-manual |
| [I] | Exhaust valve head diameter | **40 mm** | F | Same |
| [I] | Intake valve lift vs CAD | **U numeric** | U | D13-family GT model explicitly uses CAD-vs-lift arrays, but values are confidential/not printed. |
| [I] | Exhaust valve lift vs CAD | **U numeric** | U | Same |
| [I] | IVO / IVC / EVO / EVC | **U** | U | Not recovered. |
| [N] | Engine brake | Production Volvo engine brake documented; exact test-cell enablement **U** | F | Volvo D13 production literature; do not assume active during Chalmers tests. |
| [N] | Load-dependent exhaust lift / VVA | Existence documented on D13-700 family GT model; numeric profiles U | F / MODEL-DERIVED | Erlandsson 2017 Appendix A.3.10. |

The uploaded Erlandsson thesis is a **Volvo Penta D13-700**, not the selected US10 truck build. It says intake valves use CAD-vs-lift arrays and the exhaust valve model has minimum/main/boost lift profiles depending on load. This is useful evidence of family model structure, not exact US10 valve timing.  

## C. Gas exchange / CAC / EGR / turbo

### Exact selected-engine hardware

| Importance | Parameter | Value | Grade | Source |
|---|---|---|:---:|---|
| [I] | CAC | Present | D | Rijpkema et al. 2022, Sec. 2 / Fig. 2 |
| [I] | EGR route | **Long-route** | D | Rijpkema PhD, p.25, Table 3.1 |
| [I] | EGR cooler | Present in standard D13 US system | D | Latz 2016 test-rig discussion; standard cooler compared with WHR EGR boiler |
| [E] | VGT type | **Sliding-nozzle variable-geometry turbocharger** | D | Volvo D13-500/1750 production specification — https://techpub.prevostcar.com/content/media/18793/view |
| [E] | Exact turbo manufacturer/model on Chalmers engine | **U** | U | Aftermarket fitment strongly points to Holset HE451VE/HE400VG family, but exact Chalmers turbo tag was not recovered. |
| [E] | Compressor map | **U** | U | No public exact map found. |
| [E] | Turbine map | **U** | U | No public exact map found. |
| [E] | Compressor PR by test point | **U** | U | No numeric map/table recovered. |
| [E] | Turbine PR by test point | **U** | U | No numeric map/table recovered. |
| [E] | Turbine inlet temperature by test point | **U** | U | Public WHR paper gives exhaust outlet temperature, not TIT. |
| [E] | Turbo shaft speed by test point | **U** | U | No numeric values recovered. |

### Turbo family evidence

- Holset **HE451VE** is listed for 2010 Volvo D13 / Mack MP8 applications: https://turbos.com/170-032-1618/ — **F**.
- HE400VG 2835102 is listed for D13/MP8 at **335–373 kW / 450–500 hp**: https://www.rivtron.com/products/he400vg-2835102-turbocharger-d13-engine/ — **F**.
- Because these catalog families overlap, **do not select a compressor/turbine map by model name alone**. Exact OE turbo part number or engine serial number is needed.

### 1-D manifold geometry

| Importance | Parameter | Value | Grade |
|---|---|---:|:---:|
| [I] | Intake runner lengths / IDs | U | U |
| [I] | Intake manifold volume | U | U |
| [I] | Exhaust runner lengths / IDs | U | U |
| [I] | Exhaust manifold volume | U | U |
| [I] | Pulse division | Exact US10 numeric geometry U; twin-scroll/two-branch architecture exists on D13-700 family model | F topology |
| [I] | CAC internal volume | U | U |
| [I] | CAC pressure-drop curve | U | U |
| [I] | CAC outlet temperature by point | U numeric | U |

The D13-700 family GT model in Erlandsson 2017 has a **two-branch exhaust manifold feeding the twin-scroll turbine**, and a detailed intake manifold reduced from 23 GT flow volumes in the fast-running model. These are **MODEL TOPOLOGY**, not dimensioned physical hardware and must not be copied as measured dimensions.

## D. VGT / EGR control

| Importance | Parameter | Value | Grade | Note |
|---|---|---|:---:|---|
| [I] | VGT vane/rack position per point | U | U | Not published. |
| [I] | EGR valve position per point | U | U | Not published. |
| [N] | Production EGR control concept | Closed-loop EGR using Delta-P sensing with coordinated turbo/EGR valve | F | Volvo production literature for this engine family. |
| [N] | Quantitative set-points / limits | U | U | No OEM map recovered. |

**MODEL-DERIVED:** Pecoraro implemented a **PID controller on VGT rack position** and adjusted a **turbine-efficiency multiplier** to match system gauge pressures. These are GT-Power calibration devices, not physical OEM control maps. Source: Pecoraro 2013 repository abstract — https://odr.chalmers.se/items/a6ef6bc2-c853-4748-9452-7c4f62e89b89.

## E. Injection / fuel / thermal boundary conditions

| Importance | Parameter | Value | Grade | Source / note |
|---|---|---|:---:|---|
| [E] | Injection architecture | **Dual-solenoid electronic unit injection** | D | Volvo D13 US2010 production specification — https://techpub.prevostcar.com/content/media/18793/view |
| [E] | Common-rail pressure | **N/A** | D | US10–US14 Volvo D13 documented as **non-common-rail**; NHTSA-hosted Volvo service document — https://static.nhtsa.gov/odi/tsbs/2021/MC-10203697-0001.pdf |
| [E] | Maximum injection pressure | **2400 bar / 35,000 psi** | D | Volvo D13 US2010 production specification |
| [E] | Injection pressure by operating point | U | U | Maximum rating cannot substitute for dynamic unit-injector pressure. |
| [E] | Injector model | Delphi/Volvo **BEBE4G12001**, OE 21458369 / 85003658 | F | Aftermarket cross-reference; use cautiously. |
| [E] | Nozzle model | **L362TBE** | F | WUZETEM D13H/MP8 catalogue — https://www.wuzetem.pl/produkt/pl362tbe-2/ |
| [E] | Nozzle holes × diameter | **U** | U | No reliable drawing/specification found. Do not interpret catalogue “4” fields as spray-hole count. |
| [E] | SOI by point | U | U | Not published. |
| [E] | Pre/main/post quantity by point | U | U | Not published. |
| [E] | Fuel specification | ULSD, max 15 ppm sulfur | D production | Volvo D13 US2010 specification |
| [E] | Actual test-fuel LHV | U | U | Chalmers paper only says fuel was supplied from a diesel tank. |
| [E] | Actual test-fuel density | U | U | Not published. |
| [I] | Coolant temperature | U | U | No selected-point table recovered. |
| [I] | Oil temperature | U | U | No selected-point table recovered. |

---

# OUTPUTS

## 3. Exact selected-engine measurement system

Rijpkema et al. 2022 states that sensor data were sampled at **10 Hz**, written to disk every second, and **three minutes** were collected and averaged at each point. Engine speed and torque came from a Schenck D900-1e, fuel flow from an AVL 730 balance, and inlet air flow from a calibrated Venturi sufficiently upstream of the turbocharger. Source: p.3, Sec. 2 and p.4, Table 4 — https://research.chalmers.se/publication/525850/file/525850_Fulltext.pdf.

| Measurement | Instrument | Range | Accuracy | Grade |
|---|---|---:|---:|:---:|
| Engine speed | Schenck D900-1e | 0–6500 rpm | ±2 rpm | D |
| Engine torque | Schenck D900-1e | -4000–4000 Nm | ±8 Nm | D |
| Fuel flow | AVL 730 | 0–150 kg/h | ±0.9 kg/h | D |
| Exhaust pressure | WIKA A-10 | 0–2.5 bar(g) | ±0.03 bar | D |
| Venturi differential pressure | Yokogawa EJA110E | 0–5000 Pa | ±2.75 Pa | D |
| Temperature | RS Pro Type K | -75–1100 °C | ±1.5 °C | D |

**Important:** existence of a measured channel does not mean its numeric per-point values were published.

---

## 4. ESC operating points — digitised published map

The selected D13 US10 work uses the ESC A/B/C × 25/50/75/100 structure. Latz 2016 Fig. 4-16 shows these points over the D13 US10 speed range. The published 2022 Figs. 4–5 plot the engine points on speed–torque maps.

**Digitisation method:** manual read-off of the plotted operating-point markers/contours from the published figures. Approximate resolution:
- speed: ±10 rpm;
- torque: ±20–30 Nm;
- corrected exhaust mass flow: ±10–15 g/s;
- exhaust outlet temperature: ±5–10 °C.

These digitised values are D-grade with an explicit digitisation qualifier because the figure is for the exact selected engine, but they are not table-exact.

| Point | Speed rpm | Torque Nm | BMEP bar (R) | Brake power kW (R) | Corrected exhaust mass flow g/s | Exhaust outlet °C |
|---|---:|---:|---:|---:|---:|---:|
| A25 | 1200 | ~600 | 5.89 | 75.4 | ~180 | ~300 |
| A50 | 1200 | ~1200 | 11.78 | 150.8 | ~230 | ~335 |
| A75 | 1200 | ~1780 | 17.48 | 223.7 | ~300 | ~370 |
| A100 | 1200 | ~2350 | 23.07 | 295.3 | ~345 | ~410 |
| B25 | 1500 | ~600 | 5.89 | 94.2 | ~200 | ~300 |
| B50 | 1500 | ~1200 | 11.78 | 188.5 | ~270 | ~325 |
| B75 | 1500 | ~1780 | 17.48 | 279.6 | ~340 | ~355 |
| B100 | 1500 | ~2320 | 22.78 | 364.4 | ~410 | ~400 |
| C25 | 1800 | ~500 | 4.91 | 94.2 | ~210 | ~300 |
| C50 | 1800 | ~1000 | 9.82 | 188.5 | ~300 | ~320 |
| C75 | 1800 | ~1480 | 14.53 | 279.0 | ~365 | ~340 |
| C100 | 1800 | ~1900 | 18.65 | 358.1 | ~395 | ~360 |

Source: Rijpkema et al. 2022, **Figs. 4 and 5**, with speed grouping cross-checked against Latz 2016 Fig. 4-16.  
https://research.chalmers.se/publication/525850/file/525850_Fulltext.pdf  
https://publications.lib.chalmers.se/records/fulltext/231723/231723.pdf

**Exhaust-flow warning:** the paper states that the original measured engine exhaust mass flow appeared systematically high and that the Fig. 4 values were multiplied by **0.75**. Therefore these are corrected experimental values and should receive reduced weighting in calibration.

### Requested output completeness at each ESC point

| Required output | Status |
|---|---|
| [E] speed | D, digitised |
| [E] torque | D, digitised |
| [E] BMEP | R from torque and 12.8 L |
| [E] fuel flow | **U numeric** — measured with AVL 730 but values not published |
| [E] air flow | **U numeric** — measured with calibrated inlet Venturi but values not published |
| [E] BSFC | **U** — cannot derive without fuel flow |
| [E] boost | **U numeric** |
| [E] exhaust manifold pressure | **U numeric**; the paper lists an exhaust-pressure transducer but does not publish a 12-point pre-turbine table |
| [E] turbine inlet temperature | **U numeric** |
| [E] cylinder pressure | **U numeric** |
| [I/EGR-active] EGR rate | **U trustworthy numeric** |
| [I] burn rate | **U numeric** |
| [I] motoring/FMEP | **U** |
| [N] intake runner pressure trace | **U numeric** |
| [N] exhaust runner pressure trace | **U numeric** |

The exhaust outlet temperatures in the table above are **not turbine inlet temperatures** and must not be substituted for TIT.

---

## 5. Cylinder pressure / burn rate / runner pressure

Pecoraro 2013 explicitly states that dynamic pressure was measured in **cylinder 6** and in the connected **intake and exhaust runners**, and that heat-release analysis was used to derive the actual burn rate. A new cumulative burn-rate profile was then imposed in GT-Power. Source: repository abstract — https://odr.chalmers.se/items/a6ef6bc2-c853-4748-9452-7c4f62e89b89.

However:
1. Pecoraro calls the experimental engine a **similar engine** to the base model.
2. The current repository record exposes metadata/abstract but no downloadable thesis bitstream or numeric arrays.

Therefore:
- cylinder `p(θ)`: **U numeric**;
- intake-runner `p(θ)`: **U numeric**;
- exhaust-runner `p(θ)`: **U numeric**;
- burn rate / cumulative burn fraction: **U numeric**.

The uploaded Erlandsson D13-700 thesis independently confirms the same Volvo/GT workflow: measured cylinder pressure is used in a reverse run to derive burn rate, but its actual pressure arrays are also not printed.

---

## 6. EGR-rate evidence

Latz 2016 used the standard EGR-line Venturi to estimate EGR flow. The sensor is downstream of the EGR boiler after an elbow, and the pressure sensor accuracy is stated as about 2% of reading. Latz calibrated a discharge-coefficient-vs-Reynolds-number relation (Fig. 4-15). Source: pp. 57–59 / Figs. 4-14 and 4-15 — https://publications.lib.chalmers.se/records/fulltext/231723/231723.pdf.

The same thesis later reports that the EGR-flow estimate was problematic; therefore it should **not** be treated as a high-confidence validation truth. For the requested 12-point pack, EGR rate remains **U numeric**.

Standard D13 EGR cooling is also distinguishable from the WHR experimental boiler configuration: the thesis reports approximately **80 °C** after the original EGR cooler versus around **200 °C** after the experimental boiler in early tests. Those modified-WHR points should not be silently mixed with normal engine operation.

---

## 7. Full-load curve — same rating

The selected production rating is the **Volvo D13-500 / 1750**.

Documented anchor values:
- advertised / peak power: **500 hp = 373 kW** — D;
- peak torque: **1750 lb-ft at 1050 rpm = 2373 Nm** — D;
- governed speed: **2100 rpm** — D;
- start-engagement torque: **850 lb-ft at 800 rpm ≈ 1152 Nm** — D/R.

Source: Volvo D13 production specification — https://techpub.prevostcar.com/content/media/18793/view.

A plotted production torque/power curve is present in the specification, but the accessible text extraction did not expose enough graph coordinates to digitise the entire curve without inventing values. Therefore the **continuous numeric full-load curve is U**, while the published anchor points above are retained exactly.

The experimental 100%-load points from the Chalmers map are:
- A100: ~1200 rpm, ~2350 Nm, ~295 kW;
- B100: ~1500 rpm, ~2320 Nm, ~364 kW;
- C100: ~1800 rpm, ~1900 Nm, ~358 kW.

These are D-digitised experimental points, not a substitute for the OEM continuous full-load curve.

---

## 8. External family validation fallback — SwRI 2007 D13F

This is **not merged into the D13 US2010 base**. It is retained as F-grade supporting evidence.

SwRI's Heavy-Duty Diesel Engine Benchmarking Program included a **Volvo D13F**, with ESC testing, steady-state mapping, teardown/measurements and cylinder-head flowbench work.  
Source: https://www.swri.org/newsroom/press-releases/swri-benchmarking-program-evaluates-four-2007-model-year-diesel-engines.

The ICCT study states that Volvo supplied a GT-Power model and SwRI calibrated it using measured 2007 D13 data. Validation quantities included:
- **BSFC**;
- **exhaust temperature**;
- **air/fuel ratio**;
- heat-release data from **12 operating conditions**.

Source: ICCT report, p.22, “Engine Model” — https://theicct.org/sites/default/files/publications/HDVemissions_oct09.pdf.

These are useful F-grade fallback data leads, but the public report does not expose the complete numeric benchmark dataset. SwRI states that benchmarking datasets exist through its program: https://www.swri.org/markets/automotive-transportation/automotive/automotive-vehicles-engines-drivelines/heavy-duty-engine-benchmarking-program.

---

# MODEL-DERIVED / NOT HARDWARE

The following must remain separate from measured hardware:

1. **Pecoraro VGT PID controller** — model control used to adjust rack position.
2. **Pecoraro turbine-efficiency multiplier** — calibrated GT-Power multiplier.
3. **Pecoraro cumulative burn-rate profile** — pressure-derived/imposed model combustion profile.
4. **Erlandsson FRM pipe diameters, HTM and FM** — calibration variables, not measured physical dimensions.
5. **Erlandsson simplified manifold volumes** — real-time model reduction, not physical hardware.
6. **Erlandsson turbine/compressor map/rack arrays** — existence of model maps is documented, but values are not printed.
7. **FRM FMEP / friction coefficients** — model calibration inputs, not experimental friction measurements.

The uploaded Erlandsson thesis explicitly distinguishes **measurement inputs**, **simulation data**, **calibration parameters**, **design constants**, and **default constants**; this is why its tuned FRM values are not promoted into HARDWARE.

---

# 9. What can be frozen into a simulator now

### Safe or defensible reference inputs
- Bore 131 mm — D.
- Stroke 158 mm — D.
- CR 16.0 — D.
- Rod 259 mm — F fallback.
- Firing order 1-5-3-6-2-4 — F.
- Intake/exhaust valve heads 42/40 mm — F.
- Long-route cooled EGR architecture — D.
- CAC present — D.
- Sliding-nozzle VGT — D.
- Dual-solenoid electronic unit injection — D.
- Maximum injection pressure 2400 bar — D.
- No common rail — D.
- ULSD 15 ppm production fuel requirement — D production.

### Safe validation outputs
- ESC speed/torque map — D digitised.
- BMEP / brake power — R.
- Corrected exhaust mass-flow map — D digitised, low weighting because of 0.75 correction.
- Exhaust-outlet temperature map — D digitised.
- Same-rating peak power and peak torque anchors — D.

### Do **not** freeze as reference truth
- turbo maps or pointwise PR/TIT/speed;
- pointwise injection pressure/SOI/quantity;
- nozzle holes × diameter;
- actual fuel LHV/density;
- fuel flow, air flow or BSFC by point;
- boost or pre-turbine exhaust pressure by point;
- cylinder pressure / burn-rate arrays;
- EGR rate by point;
- FMEP;
- 1-D runner/manifold dimensions.

Those remain U and require either raw Chalmers/Volvo data, the Pecoraro bitstream, SwRI benchmark data, or supplier/OEM drawings.

---

# 10. Downloadable / retrievable sources located

1. **Rijpkema et al. 2022 — full PDF**  
   https://research.chalmers.se/publication/525850/file/525850_Fulltext.pdf  
   Exact selected D13 US2010 engine specification, instrumentation, Figs. 4–5 engine maps.

2. **Rijpkema doctoral thesis — full PDF**  
   https://research.chalmers.se/publication/523780/file/523780_Fulltext.pdf  
   D13 US vs D13K540, Table 3.1 including long-route EGR.

3. **Gunnar Latz 2016 doctoral thesis — full PDF**  
   https://publications.lib.chalmers.se/records/fulltext/231723/231723.pdf  
   D13 US10 WHR test rig, EGR Venturi, ESC operating-point figure, EGR cooling behavior.

4. **Igor Pecoraro 2013 — repository record**  
   https://odr.chalmers.se/items/a6ef6bc2-c853-4748-9452-7c4f62e89b89  
   Primary 12-ESC GT-Power validation study. **No downloadable bitstream exposed in current repository record.**

5. **Adam Erlandsson 2017 — Fast Running 1D model of a heavy-duty diesel engine**  
   User-supplied file: `/mnt/data/254966.pdf`  
   Volvo Penta D13-700 family model; very useful for identifying which geometry, injection, turbo, pressure and cylinder-pressure inputs existed, but values are largely normalized/confidential.

6. **Volvo D13 production specification, D13-500/1750**  
   https://techpub.prevostcar.com/content/media/18793/view  
   Exact production rating, VGT type, SOHC/4-valve head, EUI, 2400-bar max injection pressure, full-load plot.

7. **Volvo/NHTSA fuel-system service document**  
   https://static.nhtsa.gov/odi/tsbs/2021/MC-10203697-0001.pdf  
   Confirms US10–US14 D13 as non-common-rail architecture.

8. **ICCT / SwRI Volvo D13 study PDF**  
   https://theicct.org/sites/default/files/publications/HDVemissions_oct09.pdf  
   F-grade 2007 D13 benchmark; BSFC, exhaust temperature, AFR and 12-condition heat release described.

9. **SwRI benchmarking program pages**  
   https://www.swri.org/newsroom/press-releases/swri-benchmarking-program-evaluates-four-2007-model-year-diesel-engines  
   https://www.swri.org/markets/automotive-transportation/automotive/automotive-vehicles-engines-drivelines/heavy-duty-engine-benchmarking-program  
   Confirms D13F benchmarking scope and existence of benchmark datasets.

10. **WUZETEM L362TBE catalogue page**  
    https://www.wuzetem.pl/produkt/pl362tbe-2/  
    F-grade injector-nozzle family identification.

11. **Holset HE451VE 2010 Volvo D13 fitment page**  
    https://turbos.com/170-032-1618/  
    F-grade turbo family identification.

12. **HE400VG D13 450–500 hp fitment page**  
    https://www.rivtron.com/products/he400vg-2835102-turbocharger-d13-engine/  
    F-grade alternative turbo-family evidence.

---

# 11. Validation-grade status

**P5 is usable as a brake-performance / steady-state thermal validation case, but it is not complete enough to be the primary reference for combustion or turbocharger submodels.**

The hard blockers are:
1. turbo PR/TIT/shaft-speed data or exact maps;
2. numeric cylinder-pressure traces;
3. fuel-flow + air-flow data needed for BSFC/AFR;
4. injector nozzle geometry and per-point timing/quantity;
5. actual fuel LHV/density.

These are explicitly retained as U rather than back-filled with assumptions.

