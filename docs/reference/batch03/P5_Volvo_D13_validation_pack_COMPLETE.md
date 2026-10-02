# P5 — Volvo D13 heavy-duty diesel
## Complete evidence pack accumulated in this research thread

### Verdict against the original validation-role requirement

**Overall: NOT MET as a complete validation reference.**

This pack now records **all useful information gathered in the thread**, but several **[E] essential numeric inputs and outputs remain unavailable publicly**. The correct conclusion is therefore not “the research found nothing,” but:

- a strong exact-configuration chassis/engine identity and brake-load map exist;
- several required channels were **measured but their numeric values were not published**;
- a closely related Pecoraro D13 program measured cylinder and runner pressure and derived burn rate, but the numerical thesis/arrays are inaccessible;
- related D13/Penta/MP8 and SwRI programs prove that richer turbo, injector, cylinder-pressure, BSFC, AFR and EGR datasets exist, but do not expose the required values publicly;
- therefore the D13 is **useful as a partial heavy-duty brake/thermal case**, but it does **not satisfy the full heavy-duty reference slot** defined by the prompt.

The distinction between **U = not found numerically**, **measured-but-unpublished**, and **family-only data** is preserved below.

---

# 1. Configuration control

## Selected base configuration

**Volvo D13 US 2010, D13-500 / 1750 rating**

- 12.8 L inline-six
- bore × stroke: **131 × 158 mm**
- compression ratio: **16.0:1**
- peak power: **373 kW / 500 hp**
- peak torque: **2373 Nm / 1750 lb-ft**
- turbocharged, CAC, EGR
- selected because it is the richest accessible exact Chalmers experimental configuration

Primary exact source: Rijpkema et al. 2022, Table 2:  
https://research.chalmers.se/publication/525850/file/525850_Fulltext.pdf

### Mixing control

The following are **not silently merged** into the D13 US10 base:
- Pecoraro 2013 D13 GT-Power validation engine — related/similar engine.
- Volvo Penta D13-700 / Erlandsson 2017 — different 700-hp application.
- Mack MP8 — Volvo Group sister-engine family.
- SwRI 2007 Volvo D13F — earlier emissions generation.
- generic D13F/D13H workshop data — family data.

Grades:
- **D** exact selected configuration / exact experiment.
- **F** family / related build.
- **R** derived.
- **U** numeric value not found.
- **N/A** quantity does not exist for the architecture.

---

# 2. HARDWARE — exact and family-supported inputs

## A. Cranktrain / combustion chamber

| Importance | Quantity | Value | Grade | Exact source |
|---|---|---:|:---:|---|
| [E] | Bore | 131 mm | D | Rijpkema et al. 2022, Table 2 |
| [E] | Stroke | 158 mm | D | Rijpkema et al. 2022, Table 2 |
| [E] | Compression ratio | 16.0:1 | D | Rijpkema et al. 2022, Table 2 |
| [E] | Connecting-rod length | **259.0 mm** | F | Volvo Penta D13 family workshop specification, connecting-rod length entry — https://www.scribd.com/document/938147474/Groups-20-26 |
| [I] | Firing order | **1-5-3-6-2-4** | F | D13/MP8 family service literature |
| [N] | Cylinder spacing | **168 mm** | F | Volvo D13 production/service specification gathered in search |
| [N] | Piston-bowl geometry | **U** | U | No reliable bowl diameter/depth/volume recovered |

Derived:
- swept volume/cyl = `π/4 × 0.131² × 0.158 = 2.12956 L` — R
- geometric total displacement = `12.77735 L` — R
- clearance volume/cyl = `2.12956/(16-1) = 0.141971 L = 141.97 cm³` — R
- rod/stroke = `259/158 = 1.639` — R
- peak-torque BMEP = `4π×2373/0.0128 = 23.30 bar` — R

## B. Valves / valvetrain / engine brake

| Importance | Quantity | Value | Grade | Source / note |
|---|---|---:|:---:|---|
| [I] | Intake valve head diameter | **42 mm** | F | D13F/H family workshop specification — https://studylib.net/doc/27751421/volvo-d13f-engine-workshop-manual |
| [I] | Exhaust valve head diameter | **40 mm** | F | Same |
| [I] | Intake nominal cam-lobe lift | **13.1 mm** | F | Mack MP8 family service manual — https://www.manualshelf.com/manual/mack-trucks/mack-mp8-diesel-engine/service-manual-english.html |
| [I] | Exhaust nominal cam-lobe lift with engine brake | **12.5 mm** | F | Mack MP8 family service manual |
| [I] | Intake CAD-vs-lift profile | **U numeric** | U | Related D13 GT model uses explicit CAD-vs-lift arrays; values withheld |
| [I] | Exhaust CAD-vs-lift profile | **U numeric** | U | Related D13-700 GT model contains minimum/main/boost exhaust lift profiles |
| [I] | IVO / IVC / EVO / EVC | **U** | U | Not recovered |
| [N] | Engine-brake/VVA hardware | Volvo/Mack engine-brake family hardware documented | F | MP8/D13 family service literature |
| [N] | Production brake rating | ~500 hp at 2200 rpm; ~350 hp at 1500 rpm | F | Volvo D13 production specification gathered earlier |

Erlandsson 2017, Volvo Penta D13-700 (F / model evidence):
- intake valve object uses **CAD vs lift [mm] arrays**;
- exhaust valve model has **minimum, main and boost lift profiles depending on load**;
- valve reference diameter, lash and discharge-coefficient maps are design/model inputs.
Source: user-supplied `254966.pdf`, Appendix A.3.7 and A.3.10.

## C. Intake / exhaust / CAC / EGR / turbo

### Exact D13 US10 architecture

| Importance | Quantity | Value | Grade | Source |
|---|---|---|:---:|---|
| [I] | Charge-air cooler | Present | D | Rijpkema et al. 2022, schematic / Sec. 2 |
| [I] | EGR route | **Long-route** | D | Rijpkema PhD, Table 3.1 — https://research.chalmers.se/publication/523780/file/523780_Fulltext.pdf |
| [I] | EGR cooler | Present on standard configuration | D | Latz 2016 |
| [E] | VGT | **Sliding-nozzle VGT** | D production | Volvo D13-500/1750 spec — https://techpub.prevostcar.com/content/media/18793/view |
| [E] | Exact turbo OE/model | U | U | Exact Chalmers turbo tag not recovered |
| [E] | Compressor map | U | U | Not found |
| [E] | Turbine map | U | U | Not found |
| [E] | Pointwise compressor PR | U | U | Not published |
| [E] | Pointwise turbine PR | U | U | Not published |
| [E] | Pointwise turbine inlet temperature | U | U | Not published |
| [E] | Pointwise turbo speed | U | U | Not published |

Family turbo identification:
- **Holset HE451VE** listed for 2010 Volvo D13 / Mack MP8 — F — https://turbos.com/170-032-1618/
- **Holset HE400VG 2835102** listed for D13/MP8 335–373 kW / 450–500 hp — F — https://www.rivtron.com/products/he400vg-2835102-turbocharger-d13-engine/
- Because these overlap, **do not select a map by family name alone**.

### Physical 1-D pipe geometry

| Importance | Quantity | Result |
|---|---|---|
| [I] | Intake runner lengths / diameters | U |
| [I] | Intake manifold volume | U |
| [I] | Exhaust runner lengths / diameters | U |
| [I] | Exhaust manifold volume | U |
| [I] | Pulse division | F topology: D13-700 detailed GT model uses **two exhaust branches feeding a twin-scroll turbine** |
| [I] | CAC volume | U |
| [I] | CAC Δp curve | U |
| [I] | CAC outlet temperature by selected ESC point | U numeric |

Erlandsson D13-700 family evidence:
- detailed engine is a **12.8 L 6-cylinder D13-700 with twin-scroll VGT**;
- exhaust system is split into **two branches**, including port, runner, collector and flow-split volumes, each feeding a turbine scroll;
- detailed intake manifold contained **23 flow volumes** before FRM simplification;
- physical pipe length, diameters, volume and surface area existed as design constants but values are not printed.
Source: Erlandsson 2017, pp. 11–17 and Appendix A.3.2/A.3.12.

### CAC thermal evidence

Erlandsson D13-700 family model:
- measurement inputs include **RPM, injected mass, CAC outlet temperature and ambient temperatures**;
- model design constants include volume, surface area and length, but numbers are withheld.
F / model-workflow evidence.

Latz US10 experiments:
- standard production EGR cooler outlet reported around **80–100 °C**;
- early WHR-boiler configuration produced roughly **200 °C**, up to about **270 °C at full load**, causing EGR protection/closure;
- later setup reinstated original cooler and controlled EGR to about **100 °C before mixing**.
These modified-WHR configurations must be flagged and not merged with normal baseline operation.

## D. Boost / VGT / EGR control

| Importance | Quantity | Value | Grade |
|---|---|---|:---:|
| [I] | VGT rack/vane position by point | U | U |
| [I] | EGR valve position by point | U | U |
| [N] | Coordinated EGR/VGT control | documented at family/model level | F |
| [N] | Normal boost set-point map | U | U |
| [N] | EGR set-point / limits | U | U |
| [N] | High-EGR-temperature protection | EGR closes under excessive EGR temperature in Latz test | D experimental behavior |

**MODEL-DERIVED, not hardware:** Pecoraro implemented a VGT-rack PID and calibrated a turbine-efficiency multiplier. Do not treat either as an OEM control law or physical efficiency correction.

## E. Injection / fuel / thermal state

| Importance | Quantity | Value | Grade | Source / note |
|---|---|---|:---:|---|
| [E] | Injection system | **Dual-solenoid electronic unit injection** | D production | Volvo US2010 specification |
| [E] | Common-rail pressure | **N/A** | D | US10–US14 D13 is non-common-rail — https://static.nhtsa.gov/odi/tsbs/2021/MC-10203697-0001.pdf |
| [E] | Maximum injection pressure | **2400 bar / 35,000 psi** | D production | Volvo spec |
| [E] | Pointwise dynamic injection pressure | U | U | max pressure is not a substitute |
| [E] | Original injector family | **Delphi BEBE4G12001** | F | cross-reference — https://www.sunrisediesel.com/volvo-injector/ |
| [E] | Volvo OE injector | **21458369 / 85003658** | F | same cross-reference |
| [E] | Later service replacement | **85013611** | F | Volvo reman application guide — https://www.bergeystruckcenters.com/efs/wp/domains/www.bergeystruckcenters.com/wp-content/uploads/2021/09/Volvo-Reman-Technical-Application-Guide_compressed-1.pdf |
| [E] | Nozzle | **L362TBE** | F | WUZETEM D13H/MP8 catalogue — https://www.wuzetem.pl/produkt/pl362tbe/ |
| [E] | Nozzle holes × diameter | U | U | not found reliably |
| [E] | SOI by point | U | U |
| [E] | Pre/main/post quantity by point | U | U |
| [E] | Production fuel requirement | ULSD, max 15 ppm sulfur | D production | Volvo spec |
| [E] | Actual Chalmers test-fuel LHV | U | U |
| [E] | Actual Chalmers test-fuel density | U | U |
| [I] | Coolant temperature | U | U |
| [I] | Oil temperature | U | U |

The Erlandsson D13-700 model confirms that the underlying Volvo GT workflow possessed:
- rail/unit-injector pressure input;
- pre/main/post injected masses;
- pre/main/post SOI;
- injection-rate map;
- injector rate vs energizing time/pressure;
- nozzle-hole diameter, hole count and nozzle Cd as design constants.
Those **values are not printed**, so this is F / existence evidence only.

---

# 3. EXACT D13 US10 EXPERIMENTAL OUTPUTS

## Measurement system

Rijpkema et al. 2022:
- sensor data sampled at **10 Hz**;
- data written to disk every second;
- each point averaged over **3 minutes**;
- speed/torque: Schenck D900-1e;
- fuel flow: AVL 730;
- inlet air flow: calibrated Venturi upstream of turbo.

| Channel | Instrument / range | Accuracy | Grade |
|---|---|---:|:---:|
| speed | Schenck D900-1e, 0–6500 rpm | ±2 rpm | D |
| torque | Schenck D900-1e, -4000–4000 Nm | ±8 Nm | D |
| fuel flow | AVL 730, 0–150 kg/h | ±0.9 kg/h | D |
| exhaust pressure | WIKA A-10, 0–2.5 bar(g) | ±0.03 bar | D |
| Venturi Δp | Yokogawa EJA110E, 0–5000 Pa | ±2.75 Pa | D |
| temperature | Type K, -75–1100 °C | ±1.5 °C | D |
| derived engine mass flow | paper uncertainty | ±5.9% | D |

**Critical distinction:** fuel flow and air flow were definitely measured, but the accessible publication does **not print the per-point numbers**.

## ESC map — exact experiment, plotted/digitised

**Digitisation resolution**
- speed: ~±10 rpm
- torque: ~±20–30 Nm
- corrected exhaust mass flow: ~±10–15 g/s
- exhaust outlet temperature: ~±5–10 °C

| Point | rpm | measured torque Nm | BMEP bar R | brake power kW R | corrected exhaust mass flow g/s | measured exhaust-outlet °C |
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

Source: Rijpkema et al. 2022, Figs. 4–5 — https://research.chalmers.se/publication/525850/file/525850_Fulltext.pdf

**These are not merely input operating points.**
- speed/load grid = imposed test condition;
- torque = experimental output;
- exhaust mass flow = experimental result after authors' correction;
- exhaust outlet temperature = experimental output.

### Exhaust-flow correction

The paper states the measured exhaust mass flow appeared systematically high and the plotted values were multiplied by **0.75**. Therefore the table above contains the **authors' corrected experimental flow**, not untouched raw flow. Treat with lower validation weight.

### Output requirement status by ESC point

| Requirement | Status | Interpretation |
|---|---|---|
| [E] speed | D | available |
| [E] torque | D digitised | available |
| [E] BMEP | R | available |
| [E] fuel flow | **measured-but-unpublished; numeric U** | essential blocker |
| [E] air flow | **measured-but-unpublished; numeric U** | essential blocker |
| [E] BSFC | U | cannot derive without fuel flow |
| [E] boost | U numeric | essential blocker |
| [E] exhaust-manifold pressure | U numeric | essential blocker |
| [E] TIT | U numeric | essential blocker |
| [E] cylinder pressure | U numeric | essential blocker |
| [I/EGR-active] EGR rate | U trustworthy numeric | blocker at EGR-active points |
| [I] burn rate | U numeric | blocker for combustion validation |
| [I] FMEP/motoring | U | unavailable |
| [N] runner pressure traces | U numeric | related study measured them |

The published exhaust-outlet temperature is **not TIT** and is not relabelled.

---

# 4. EGR MEASUREMENTS

Latz 2016 used the standard EGR-line Venturi as an EGR-flow sensor and experimentally calibrated its discharge coefficient.

### Digitised EGR Venturi Cd(Re)
Stated digitisation resolution: approximately ΔRe ≈ 10,000 and ΔCd ≈ 0.02.

| Re | Cd |
|---:|---:|
| ~17,000 | ~0.76 |
| ~30,000 | ~0.85 |
| ~43,000 | ~0.89 |
| ~56,000 | ~0.91 |
| ~70,000 | ~0.91 |
| ~80,000 | ~0.94 |
| ~100,000 | ~0.97 |
| ~110,000 | ~0.97 |
| ~122,000 | ~1.02 |
| ~130,000 | ~0.97 |
| ~160,000 | ~0.97 |
| ~177,000 | ~0.94 |

Source: Latz 2016, Fig. 4-15 — https://publications.lib.chalmers.se/records/fulltext/231723/231723.pdf

**Quality warning:** later energy-balance checks indicated the Venturi-based EGR flow **overestimated EGR flow by roughly 30% in more than two-thirds of cases**, especially at high load. Researchers subsequently estimated EGR flow from the heat balance. Therefore the raw Venturi EGR-rate result is **not accepted as validation truth**.

---

# 5. CYLINDER PRESSURE / BURN RATE / RUNNER PRESSURE

## Pecoraro 2013 D13 validation program — related build

The repository abstract explicitly states:
- dynamic pressure measured in **cylinder 6**;
- dynamic pressure measured in the associated **intake runner**;
- dynamic pressure measured in the associated **exhaust runner**;
- heat-release analysis used to derive actual burn rate;
- cumulative burn-rate profile imposed into GT-Power;
- validation covered the **12 loaded ESC points**.

Source: https://odr.chalmers.se/items/a6ef6bc2-c853-4748-9452-7c4f62e89b89

But:
- engine is described as a **similar engine**;
- current ODR record exposes metadata/abstract but no thesis bitstream;
- numerical pressure/burn-rate arrays therefore remain **U**.

## Erlandsson 2017 independent Volvo D13-family confirmation

The D13-700 GT workflow explicitly uses measured cylinder pressure in a reverse run:
- cylinder pressure = input;
- burn rate = calculated output;
- iterative heat-transfer correction is then applied.
Source: user-supplied `254966.pdf`, Appendix A.3.9.1.1.

This confirms that the data type existed in the Volvo/Chalmers D13 workflow, but **does not supply the values**.

---

# 6. D13-700 / VirCal DATASET EVIDENCE — FAMILY ONLY

Erlandsson 2017 is useful because it reveals the scale of the hidden Volvo dataset:

- Volvo Penta **D13-700**, 12.8 L I6;
- **169 steady-state test-cell operating points** in a part-load-map speed sweep;
- 27 representative points selected for FRM work;
- engine range about **600–2100 rpm**;
- stated maximum torque about **2500 Nm** and power **700 hp**;
- measured reference properties include:
  - compressor inlet/outlet pressures;
  - turbine inlet/outlet pressures;
  - compressor inlet/outlet temperatures;
  - turbine inlet/outlet temperatures;
  - cold-side air mass flow;
  - turbo/turbine speed input;
  - cylinder-pressure traces for combustion calibration;
  - injected-fuel quantities and timing inputs in the detailed model workflow.

**All absolute values were normalized/withheld for confidentiality.**  
This is **F evidence of data existence**, not a numeric replacement for the D13 US10 outputs.

---

# 7. SwRI / ICCT D13F BENCHMARK — EXTERNAL FAMILY FALLBACK

Separate **2007 Volvo D13F** benchmark at Southwest Research Institute.

Publicly documented benchmark content:
- ESC testing;
- steady-state engine mapping;
- teardown/measurement and cylinder-head flowbench work;
- GT-Power model calibrated to measured D13 data;
- measured **BSFC**;
- measured **exhaust temperature**;
- measured **air/fuel ratio**;
- measured **EGR flow** used in calibration;
- **heat-release data from 12 operating conditions**;
- production torque/power reference.

Sources:
- SwRI program: https://www.swri.org/markets/automotive-transportation/automotive/automotive-vehicles-engines-drivelines/heavy-duty-engine-benchmarking-program
- SwRI D13F benchmark announcement: https://www.swri.org/newsroom/press-releases/swri-benchmarking-program-evaluates-four-2007-model-year-diesel-engines
- ICCT report: https://theicct.org/sites/default/files/publications/HDVemissions_oct09.pdf

This is **F only** because it is a 2007 D13F, not the selected 2010 D13H/US10. The public report does not expose the complete numeric benchmark dataset.

---

# 8. SAME-RATING FULL-LOAD DATA

Volvo D13-500 / 1750 production rating:
- peak power: **500 hp = 373 kW** — D
- peak torque: **1750 lb-ft = 2373 Nm at 1050 rpm** — D
- governed speed: **2100 rpm** — D
- start-engagement torque: **850 lb-ft ≈ 1152 Nm at 800 rpm** — D/R
- recommended cruise: **1300–1500 rpm** — D production

Source: Volvo D13 production specification — https://techpub.prevostcar.com/content/media/18793/view

The specification includes a full-load power/torque plot, but a reliable full numeric digitisation was not recovered in this research thread. Continuous curve = U; anchor points above = D.

Exact Chalmers near/full-load measured points from ESC map:
- A100: ~1200 rpm, ~2350 Nm, ~295 kW
- B100: ~1500 rpm, ~2320 Nm, ~364 kW
- C100: ~1800 rpm, ~1900 Nm, ~358 kW

---

# 9. MODEL-DERIVED — MUST NOT BE REPORTED AS HARDWARE

- Pecoraro VGT-rack PID.
- Pecoraro turbine-efficiency multiplier.
- Pecoraro pressure-derived cumulative burn-rate profile.
- Erlandsson FRM reduced/lumped intake/exhaust volumes.
- Erlandsson tuned pipe orifice diameters.
- Erlandsson HTM/FM calibration multipliers.
- Erlandsson compressor/turbine map calibration multipliers.
- FRM FMEP/peak-cylinder-pressure/mean-piston-speed calibration factors.
- EGR PID/controller maps used in the real-time model.

Erlandsson explicitly separates **measurement inputs**, **simulation data**, **calibration parameters**, **design constants**, and **default constants**. This pack preserves that separation.

---

# 10. Requirement scorecard

## Inputs

| Required [E] input | Status |
|---|---|
| rod length | **F 259 mm — usable fallback** |
| compression ratio | **D 16.0:1** |
| turbo PR/TIT/speed or maps | **U — FAIL** |
| injector holes × diameter | **U — FAIL** |
| injection pressure/SOI/quantity by point | **U — FAIL** |
| fuel LHV/density | **U — FAIL** |

## Outputs

| Required [E] output | Status |
|---|---|
| speed | **D** |
| torque/BMEP | **D/R** |
| fuel flow | **measured but numeric U — FAIL** |
| air flow | **measured but numeric U — FAIL** |
| BSFC | **U — FAIL** |
| boost | **U — FAIL** |
| exhaust-manifold pressure | **U — FAIL** |
| TIT | **U — FAIL** |
| cylinder pressure | **measured in related D13 study but numeric U — FAIL** |

**Therefore the original P5 reference-role criterion is not met.**

What *is* usable:
- exact large-bore geometry;
- 12-point exact brake-load map;
- corrected exhaust mass-flow map;
- exhaust-outlet-temperature map;
- same-rating full-load anchors;
- good family hardware constraints.

What is *not* usable without defaults:
- turbo validation;
- combustion/indicated-cycle validation;
- fuel-consumption validation;
- friction separation.

---

# 11. Source / file inventory gathered in this thread

1. Rijpkema et al. 2022 exact D13 US10 experiment — https://research.chalmers.se/publication/525850/file/525850_Fulltext.pdf
2. Rijpkema 2018 D13 US GT model — https://research.chalmers.se/publication/504522/file/504522_Fulltext.pdf
3. Rijpkema doctoral thesis — https://research.chalmers.se/publication/523780/file/523780_Fulltext.pdf
4. Latz 2016 doctoral thesis — https://publications.lib.chalmers.se/records/fulltext/231723/231723.pdf
5. Pecoraro 2013 ODR record — https://odr.chalmers.se/items/a6ef6bc2-c853-4748-9452-7c4f62e89b89
6. Erlandsson 2017 D13-700 thesis — user upload `/mnt/data/254966.pdf`
7. Volvo D13-500/1750 production specification — https://techpub.prevostcar.com/content/media/18793/view
8. Alternate Prevost/Volvo technical document — https://techpub.prevostcar.com/content/media/18795/view
9. Volvo/NHTSA fuel-system document — https://static.nhtsa.gov/odi/tsbs/2021/MC-10203697-0001.pdf
10. Mack MP8 service manual — https://www.manualshelf.com/manual/mack-trucks/mack-mp8-diesel-engine/service-manual-english.html
11. Mack MP8 valve/injector adjustment manual — https://www.manualshelf.com/manual/mack-trucks/mack-mp8-valves-and-engine-injectors-adjustment-manual/user-manual-english.html
12. Volvo D13F/H workshop manual mirror — https://studylib.net/doc/27751421/volvo-d13f-engine-workshop-manual
13. Volvo Penta D13 workshop specification — https://www.scribd.com/document/938147474/Groups-20-26
14. Holset HE451VE D13 fitment — https://turbos.com/170-032-1618/
15. HE400VG D13 450–500 hp fitment — https://www.rivtron.com/products/he400vg-2835102-turbocharger-d13-engine/
16. WUZETEM L362TBE nozzle listing — https://www.wuzetem.pl/produkt/pl362tbe/
17. D13 injector cross-reference — https://www.sunrisediesel.com/volvo-injector/
18. Volvo reman injector application guide — https://www.bergeystruckcenters.com/efs/wp/domains/www.bergeystruckcenters.com/wp-content/uploads/2021/09/Volvo-Reman-Technical-Application-Guide_compressed-1.pdf
19. SwRI heavy-duty benchmarking program — https://www.swri.org/markets/automotive-transportation/automotive/automotive-vehicles-engines-drivelines/heavy-duty-engine-benchmarking-program
20. SwRI 2007-model-year D13F benchmark announcement — https://www.swri.org/newsroom/press-releases/swri-benchmarking-program-evaluates-four-2007-model-year-diesel-engines
21. ICCT D13/GT-Power benchmark report — https://theicct.org/sites/default/files/publications/HDVemissions_oct09.pdf

---

# 12. Recommended disposition

Under the original acceptance criterion, **P5 should be marked PARTIAL / NOT MET**, not “complete.”

Keep it if the project benefits from:
- a 12.8-L heavy-duty geometry case;
- brake-load and exhaust thermal validation;
- VGT/EGR architecture reference.

But if the heavy-duty slot specifically requires **fuel/air/BSFC + turbo boundaries + numeric cylinder pressure at common operating points**, then a **replacement or a restricted-role label is required** unless the unpublished Chalmers/Volvo/SwRI data can be obtained.

