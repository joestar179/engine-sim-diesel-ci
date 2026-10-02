# Stage 1 Research Shortlist — Engines with Public Simulator Inputs and Measured Performance

## Scope

Purpose: identify engines suitable for validating a 0-D / quasi-dimensional Engine Simulator using public physical inputs and measured outputs.

Scoring:
- **D** = published for the exact engine/configuration
- **P** = partially published, indirect, figure-only where important details remain unresolved, or closely related configuration
- **U** = not found in the bounded Stage-1 search
- **—** = not applicable

Priority is placed on **B, C, H, and I**, because these are usually the hardest items to obtain.

Required inputs:
- **A. Geometry** — bore, stroke, rod length, compression ratio/clearance volume, cylinder count/layout, firing order
- **B. Valvetrain** — valve diameters, lift profile or max lift + timing, cam timing/VVT, valve count
- **C. Breathing** — intake runners, plenum, throttle, exhaust primary/secondary geometry, muffler/back-pressure, flow coefficients where available
- **D. Combustion/Fuel** — fuel properties; SI spark/λ; diesel injection timing, pressure/rate, nozzle holes × diameter
- **E. Heat transfer/Thermal** — coolant/oil temperatures; ideally wall temperatures
- **F. Turbo system** — compressor/turbine maps or measured pressure ratio/boost/TIT, intercooler effectiveness

Required outputs:
- **G. Performance** — measured torque/power curve and preferably part-load points
- **H. Fuel + airflow** — BSFC/fuel consumption and air flow at the same points
- **I. Cylinder pressure** — pressure traces or burn-rate / heat-release data
- **J. Other validation** — exhaust temperature, manifold pressure, friction/FMEP if available

---

## Slot 1 — Small SI, single cylinder, approximately 100–500 cc

| Rank | Candidate | Engine & lab | Sources | Data access / numeric form | A | B | C | D | E | F | G | H | I | J | Notes / main gaps |
|---|---|---|---|---|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|---|
| **1** | **Honda GX160** | Honda GX160, various university dynamometer / GT-Power studies | [1-D simulation / pressure-model study](https://www.combustion-engines.eu/pdf-200067-123531?filename=1D+simulation+of+the.pdf); [manifold optimisation / dyno study](https://www.researchgate.net/publication/306407837_PARAMETERS_OPTIMIZATION_OF_MINIMOTOR) | Open papers; mainly tables + plotted curves rather than one raw dataset | D | D | P | P | P | — | D | P | D | P | Very promising practical Engine Simulator target. Studies include valve timing / flow treatment, calibrated cylinder pressure, torque/power and BSFC. Intake/exhaust optimisation papers provide actual pipe dimensions. Main unresolved issue is assembling one exact test configuration and obtaining complete stock breathing geometry plus airflow. |
| **2** | **University of Michigan TCC-III** | 571.7 cc single-cylinder optical SI engine, University of Michigan / GM | [TCC-III public downloads](https://volker-sick.engin.umich.edu/home/downloads); [TCC-III CFD input dataset](https://www.researchgate.net/publication/279265203_TCC-III_CFD_Input_Dataset); [benchmark paper](https://www.researchgate.net/publication/283334259_TCC-III_engine_benchmark_for_large-eddy_simulation_of_IC_engine_flows) | **Excellent open downloadable files**: STL/IGES geometry, valve-lift curves, GT-Power model and experimental data | D | **D** | **D** | P | D | — | U | P/D | **D** | D | Probably the strongest public gas-exchange / combustion benchmark found. It is slightly above the nominal displacement range and is not a conventional production-engine WOT/BSFC benchmark. |
| **3** | **Ricardo Hydra / Rover K-series head** | Ricardo Hydra single-cylinder research engine, University of Alberta | [University of Alberta thesis](https://sites.ualberta.ca/~ckoch/thesis/RLthesis_hyper.pdf) | Open thesis; tables + figures; no raw downloadable dataset identified | D | D | P | P | P | — | P | D/P | **D** | P | Approximately 0.45 L, with bore/stroke/rod/CR, valve lift and events documented. Intake air and cylinder pressure are measured. External runner/plenum/exhaust geometry is less completely exposed than TCC-III. |

### Slot 1 recommendation

- **Honda GX160** if the objective is an engine with useful brake performance and BSFC.
- **TCC-III** if the objective is validating gas dynamics, valve motion and in-cylinder pressure prediction.

These are complementary benchmark types rather than direct substitutes.

---

## Slot 2 — Typical automotive NA SI, inline-4, approximately 1.6–2.5 L

| Rank | Candidate | Engine & lab | Sources | Data access / numeric form | A | B | C | D | E | F | G | H | I | J | Notes / main gaps |
|---|---|---|---|---|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|---|
| **1** | **2014 Mazda 2.0 SkyActiv-G** | EPA NVFEL / FEV benchmark engine | [EPA benchmarking page](https://www.epa.gov/vehicle-and-fuel-emissions-testing/benchmarking-advanced-low-emission-light-duty-vehicle-technology); [EPA/SAE 1-D validation study](https://19january2021snapshot.epa.gov/sites/static/files/2016-10/documents/2016-01-0565-air-flow-optim-calib-nat-asp-eng.pdf); [fuel-map generation documentation](https://19january2021snapshot.epa.gov/sites/static/files/2016-11/documents/procs-gen-eng-fuel-cons-map-2014-mazda-2-0l-skyact.pdf) | **Open spreadsheets / CSV test data plus PDFs/plots** | D | P/D | **P** | D | P | — | **D** | **D** | **D** | D | Strongest automotive SI candidate found. EPA measured airbox/plenum volumes, intake/exhaust geometry and valve cam profiles for GT-Power work and acquired crank-angle pressures. Critical Stage-2 question: whether all measured breathing dimensions and lift data are numerically published rather than only described as having been measured. |
| **2** | **Ford 2.0 Zetec experimental GT-Power project** | Ford Zetec, academic/project work | [Detailed GT-Power build documentation](https://nwmobilemechanicdotcom.wordpress.com/mechanic-research-papers/) | Public web document; numerical tables + figures, but weaker provenance than a university repository | D | **D** | **D/P** | P | U/P | — | P | P | U | U/P | Unusually detailed hard geometry: runner dimensions, port dimensions, plenum areas, valve diameters, lift and duration. Weakness is source provenance and lack of a comparably strong pressure/BSFC dataset. |
| **3** | **2013 Chevrolet 2.5 Ecotec LCV** | EPA NVFEL production benchmark | [EPA benchmark package](https://www.epa.gov/vehicle-and-fuel-emissions-testing/benchmarking-advanced-low-emission-light-duty-vehicle-technology); [EPA ALPHA maps](https://www.epa.gov/vehicle-and-fuel-emissions-testing/combining-data-complete-engine-alpha-maps) | **Open ZIP/spreadsheets + ALPHA map files** | D | U/P | U | D | P | — | **D** | **D** | P | D/P | Excellent brake/fuel/airflow benchmark but poor public physical breathing definition. Same general failure mode as the LV3: outputs are much better exposed than B/C. |

### Slot 2 recommendation

**2014 Mazda 2.0 SkyActiv-G** is the clear Stage-2 priority.

Before accepting it as a fully reproducible simulator engine, Stage 2 must determine whether the actual measured runner, plenum, port and valve-profile numbers are public, not merely referenced inside EPA's internal GT-Power model.

---

## Slot 3 — Turbocharged + intercooled SI, approximately 1.5–2.5 L

| Rank | Candidate | Engine & lab | Sources | Data access / numeric form | A | B | C | D | E | F | G | H | I | J | Notes / main gaps |
|---|---|---|---|---|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|---|
| **1** | **2013 Ford 1.6 EcoBoost** | Ford 1.6 L TGDI; EPA NVFEL + academic combustion modelling | [EPA benchmark packages](https://19january2021snapshot.epa.gov/vehicle-and-fuel-emissions-testing/benchmarking-advanced-low-emission-light-duty-vehicle-technology_.html); [1.6 EcoBoost GT-Power / cylinder-pressure work](https://www.researchgate.net/publication/321414428_Combustion_Model_for_a_Homogeneous_Turbocharged_Gasoline_Direct-Injection_Engine); [EPA ALPHA maps](https://www.epa.gov/vehicle-and-fuel-emissions-testing/combining-data-complete-engine-alpha-maps) | **Open EPA spreadsheets/maps plus model/pressure figures** | D | P | P | D | P | **P** | D | **D** | **D** | D | Strongest turbo-SI lead found. Main unresolved issue is the turbo: no verified public production compressor+turbine map and no fully demonstrated numerical gas-path geometry. |
| **2** | **2016 Honda L15B7 1.5 Turbo** | Honda Civic L15B7; EPA NCAT/NVFEL | [EPA technical paper](https://www.epa.gov/sites/default/files/2019-04/documents/sae-2018-01-0319-benchmarking-2016-honda-civic-1.5-liter-l15b7-turbocharged-engine.pdf); [EPA test-data packages](https://19january2021snapshot.epa.gov/vehicle-and-fuel-emissions-testing/benchmarking-advanced-low-emission-light-duty-vehicle-technology_.html); [ALPHA package](https://www.epa.gov/vehicle-and-fuel-emissions-testing/combining-data-complete-engine-alpha-maps) | **Open spreadsheet packages**, with many plotted results in paper | D | P | U/P | **D** | D | P | **D** | **D** | P | **D** | Excellent measured benchmark: torque, fuel flow, airflow and charge temperatures. Complete public production turbo maps and detailed intake/exhaust geometry remain missing. |
| **3** | **2016 Mazda 2.5 Turbo SkyActiv-G** | EPA NVFEL | [EPA test packages](https://19january2021snapshot.epa.gov/vehicle-and-fuel-emissions-testing/benchmarking-advanced-low-emission-light-duty-vehicle-technology_.html); [ALPHA packages](https://19january2021snapshot.epa.gov/vehicle-and-fuel-emissions-testing/combining-data-complete-engine-alpha-maps_.html) | **Open ZIP/spreadsheet** | D | P | U/P | D | P | P | **D** | **D** | P | D | Output-rich, but no equivalent public evidence of complete compressor/turbine maps or entire intake/exhaust geometry. |

### Slot 3 recommendation

No candidate in this bounded pass meets A–J completely.

**Ford 1.6 EcoBoost** is the strongest candidate to investigate further because exact-engine EPA performance data can be connected to GT-Power / cylinder-pressure research.

The systematic blocker is **F — turbocharger maps and full gas-path geometry**.

---

## Slot 4 — V-configuration SI

| Rank | Candidate | Engine & lab | Sources | Data access / numeric form | A | B | C | D | E | F | G | H | I | J | Notes / main gaps |
|---|---|---|---|---|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|---|
| **1** | **Ford Racing 302 V8** | Ford 302 V8, Metropolia UAS | [Ford 302 Engine Simulation thesis](https://www.theseus.fi/bitstream/10024/60505/1/Oppari.pdf) | Open thesis; mainly tables / figures; no raw dataset identified | D/P | P/D | P | P | U | — | **D** | U/P | U | U/P | Best older/V8 lead found. A GT-Power model was built and compared to dynamometer results. BSFC/airflow and pressure-trace coverage remain weak and Stage 2 must determine exactly how much cam/manifold detail is numerically disclosed. |
| **2** | **2014 Chevrolet 4.3 Ecotec3 LV3 V6** | EPA NVFEL | [EPA exact-engine test package](https://19january2021snapshot.epa.gov/vehicle-and-fuel-emissions-testing/benchmarking-advanced-low-emission-light-duty-vehicle-technology_.html); [ALPHA map package](https://www.epa.gov/vehicle-and-fuel-emissions-testing/combining-data-complete-engine-alpha-maps) | **Open test spreadsheets + map package** | D | P/U | **U** | D | P | — | **D** | **D** | P | D | Very strong validation outputs but not a solution to the proprietary breathing-geometry problem. |
| **3** | **2015 Ford 2.7 EcoBoost V6** | EPA NVFEL | [EPA 2.7 EcoBoost packages](https://www.epa.gov/vehicle-and-fuel-emissions-testing/benchmarking-advanced-low-emission-light-duty-vehicle-technology); [ALPHA package](https://www.epa.gov/vehicle-and-fuel-emissions-testing/combining-data-complete-engine-alpha-maps) | **Open ZIP/spreadsheet** | D | P/U | U | D | P | P | **D** | **D** | P | D | Useful EFI/turbo V6 performance benchmark, but B/C and turbo maps remain unresolved. |

### Slot 4 recommendation

This is one of the weakest slots.

Do **not** select a V-engine merely to fill the category. Doing so would reproduce the proprietary-geometry problem that motivated this search.

---

## Slot 5 — Small / medium diesel, preferably mechanical injection

| Rank | Candidate | Engine & lab | Sources | Data access / numeric form | A | B | C | D | E | F | G | H | I | J | Notes / main gaps |
|---|---|---|---|---|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|---|
| **1** | **Kirloskar TV1 661 cc DI** | Kirloskar TV1 university laboratory engine | [Experimental specifications/results](https://www.sciencedirect.com/org/science/article/pii/S1555256X22000996); [detailed TV1 appendix/specification](https://www.scribd.com/document/376343478/11-Appendix); [instrumented setup](https://pubs.acs.org/doi/suppl/10.1021/acsomega.3c02782/suppl_file/ao3c02782_si_001.pdf) | Mostly papers / PDFs with tables and plots; no unified raw archive identified | **D** | **P** | P/U | **D** | P | — | **D** | P/D | **D** | P/D | Promising mechanical-injection target. Published information includes 87.5×110 mm, CR 17.5, rod length, valve diameter/lift, MICO inline pump with mechanical governor, injection timing and opening pressure. Main weakness is complete lift-vs-angle and manifold/exhaust definition. |
| **2** | **AVL 5402 511 cc single-cylinder diesel** | AVL 5402 research engine, multiple university labs | [AVL 5402 manual copy](https://www.scribd.com/document/857955153/5402-030-Manual); [experimental engine/fuel specification](https://journals.sagepub.com/doi/10.1177/0954407014548737); [open combustion study](https://www.mdpi.com/1996-1073/16/1/139) | Manual + open papers; manual contains detailed numerical build information | **D** | **D/P** | P | **D** | **D** | — | P | P/D | **D** | D/P | Very well documented physical research engine. Main mismatch is technology: common rail rather than mechanical injection. External air/exhaust arrangement varies among laboratories. |

### Slot 5 recommendation

- **Kirloskar TV1** = better technology match for a mechanically injected diesel.
- **AVL 5402** = superior engineering documentation.

Both merit Stage-2 investigation.

---

## Slot 6 — Common-rail diesel, turbocharged, approximately 1.5–3.0 L

| Rank | Candidate | Engine & lab | Sources | Data access / numeric form | A | B | C | D | E | F | G | H | I | J | Notes / main gaps |
|---|---|---|---|---|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|---|
| **1** | **GM/Opel 1.9 L diesel / ECN Small-Bore Diesel** | GM 1.9L-derived research engine; Sandia Engine Combustion Network | [ECN Small-Bore Diesel repository](https://ecn.sandia.gov/engines/small-bore-diesel-engine/); [ECN CFD geometry/data](https://ecn.sandia.gov/engines/small-bore-diesel-engine/cfd/) | **Outstanding open downloadable CAD, valve curves, flow coefficients and experimental data** | **D** | **D** | **D/P** | **D** | P | **P** | P | D/P | **D** | **D** | Diesel analogue of TCC-III. Full-engine STL/STEP, piston geometry, valve-lift curves, valve flow coefficients, spray targeting and experimental validation are public. External production plumbing and complete public turbo maps remain the main limitations. |
| **2** | **BMW N57 3.0 diesel** | EPA NVFEL | [EPA test-data package](https://www.epa.gov/vehicle-and-fuel-emissions-testing/benchmarking-advanced-low-emission-light-duty-vehicle-technology); [EPA ALPHA map](https://www.epa.gov/vehicle-and-fuel-emissions-testing/combining-data-complete-engine-alpha-maps) | **Open large test package + map spreadsheets/files** | D | P/U | U | D/P | P | P | **D** | **D** | P | D | One of the best public commercial-diesel output datasets, but much weaker than ECN in physical input completeness. Better as an independent validation engine than as the first implementation engine. |
| **3** | **2020 GM 3.0 Duramax / SwRI GT-Power model** | GM 3.0 I6 diesel, SwRI / Argonne / EPA | [EPA/ALPHA map package](https://www.epa.gov/vehicle-and-fuel-emissions-testing/combining-data-complete-engine-alpha-maps); [EPA model description](https://nepis.epa.gov/Exe/ZyPURL.cgi?Dockey=P1019VPM.txt) | Public mapped-output package; underlying detailed SwRI model/test data are not fully public | P | U | U/P | P | P | P | **D/P** | **D/P** | P | P | Attractive modern architecture, but the underlying model is calibrated to private SwRI/OEM information. The public efficiency map is not equivalent to a public model deck. |

### Slot 6 recommendation

**GM/Opel 1.9 L / ECN Small-Bore Diesel** is the clear winner.

It is one of the few engines found where the difficult **B/C/I** information is intentionally published to permit independent model reproduction.

---

## Slot 7 — Heavy-duty / large diesel, approximately 7–15 L

| Rank | Candidate | Engine & lab | Sources | Data access / numeric form | A | B | C | D | E | F | G | H | I | J | Notes / main gaps |
|---|---|---|---|---|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|---|
| **1** | **Volvo D13** | Volvo D13 heavy-duty diesel, Chalmers | [Chalmers GT-Power validation thesis](https://odr.chalmers.se/items/a6ef6bc2-c853-4748-9452-7c4f62e89b89) | **Open MSc thesis**; measurements mainly tables/figures; no raw GT model identified | P/D | P/U | **P** | P | D/P | **P** | D/P | D/P | **D** | **D** | Strongest heavy-duty lead. GT-Power was validated against 12 ESC points. Dynamic cylinder pressure and pressures in connected intake/exhaust runners were measured; burn rate was reconstructed; charge/exhaust coolers and VGT were calibrated. Public reproducibility of valve-lift arrays, complete pipe geometry and VGT maps is not yet established. |
| **2** | **FPT Cursor 11, 11.0 L** | FPT / Politecnico di Torino heavy-duty research engine | [GT-Power DI-Pulse MSc thesis](https://webthesis.biblio.polito.it/18543/); [FPT/Polito experimental paper](https://www.mdpi.com/1996-1073/12/24/4704); [pressure comparison work](https://www.politesi.polimi.it/retrieve/02807a69-9f64-4422-b12e-b117c068d47a/2022_07_Castorina.pdf) | **Open thesis + papers**; extensive test plots/tables, but no complete public engine CAD/model deck identified | P | U | U/P | **D/P** | P | P | **D/P** | **D** | **D** | **D** | Rich combustion/output validation including injection quantity/SOI, rail pressure and cylinder pressure over many conditions. Proprietary FPT geometry underlying the GT-Power model is not fully reproduced publicly. |

### Slot 7 recommendation

**Volvo D13** is the strongest heavy-duty lead from this bounded pass.

The search was intentionally stopped at two credible engines rather than padding the slot with weak Caterpillar/Cummins/locomotive candidates lacking the requested breathing or validation data.

---

# Stage-1 shortlist

| Intended validation role | Preferred candidate | Reason |
|---|---|---|
| Small conventional SI | **Honda GX160** | Best mix of brake performance, BSFC, cylinder pressure and accessible mechanical/cam information. |
| Fundamental SI model benchmark | **UM TCC-III** | Excellent public CAD, valve motion, GT-Power model and experimental pressure/flow information. |
| Automotive NA SI | **2014 Mazda 2.0 SkyActiv-G** | Strong EPA dyno/airflow/fuel/pressure package plus documented physical measurement for a 1-D model. |
| Turbo SI | **Ford 1.6 EcoBoost** | Best connection found between EPA performance data and exact-engine GT-Power / pressure research. |
| Older / V-engine SI | **Ford 302 V8** | Best credible older V-engine 1-D + dynamometer lead, but not complete enough yet. |
| Mechanical small diesel | **Kirloskar TV1** | Mechanical inline injection and governor, cylinder pressure and many dynamometer studies. |
| Highly documented small research diesel | **AVL 5402** | Strong engineering/manual data including valves, injector and thermal conditions. |
| Modern CR diesel research benchmark | **GM 1.9 L / ECN** | Outstanding public CAD, valve lift, flow coefficients, spray geometry and validation data. |
| Heavy-duty diesel | **Volvo D13** | Strongest open 1-D/experimental heavy-duty validation study found. |

---

# Stage-2 priority set

The following engines justify full extraction:

1. **University of Michigan TCC-III**
2. **Honda GX160**
3. **2014 Mazda 2.0 SkyActiv-G**
4. **Kirloskar TV1**
5. **AVL 5402**
6. **GM/Opel 1.9 L / ECN Small-Bore Diesel**
7. **Volvo D13**

## Highest-confidence open physics benchmarks

### University of Michigan TCC-III

TCC-III is one of the strongest public SI modelling benchmarks because its public-data philosophy aligns directly with independent simulation reproduction. Geometry, valve motion, a GT-Power representation and experimental flow/pressure data are available.

Its principal weakness is that it is not a conventional production-engine full-load torque/BSFC benchmark.

### GM/Opel 1.9 L / ECN Small-Bore Diesel

The ECN engine is the equivalent standout on the diesel side. CAD, valve-lift curves, flow coefficients, spray targeting and experimental combustion information are intentionally released for simulation comparison.

The remaining gaps concern complete external production plumbing and turbocharger maps rather than the cylinder/head/valve model.

## Production-engine priority

### 2014 Mazda 2.0 SkyActiv-G

This is the strongest production automotive candidate identified.

EPA reports that it physically measured:
- airbox and plenum volumes,
- intake/exhaust geometry,
- valve cam profiles,
- airflow behaviour,
- crank-angle cylinder/intake/exhaust pressures,

and collected a large set of dynamometer speed/load points with fuel consumption.

The decisive Stage-2 question is:

> Are the actual measured runner, plenum, port and valve-profile numerical data publicly recoverable, or were they used internally in EPA's GT-Power model without full publication?

If the numbers are not public, the Mazda should **not** be reverse-fitted to its measured performance. It should instead remain an output-validation engine.

---

# Main Stage-1 conclusion

The bounded search did **not** identify a turbocharged production engine for which every required category **A–J** is demonstrably public.

The recurring blockers are:

1. **Valve lift and detailed cam data**
2. **Complete intake/exhaust geometry**
3. **Matched airflow + BSFC datasets**
4. **Cylinder-pressure traces**
5. **Turbocharger compressor and turbine maps**

The strongest deliberately open research benchmarks are:

- **University of Michigan TCC-III — SI**
- **GM/Opel 1.9 L / ECN Small-Bore Diesel — diesel**

The strongest production-engine candidate requiring Stage-2 verification is:

- **2014 Mazda 2.0 SkyActiv-G**

Commercial EPA benchmark engines remain extremely valuable for measured outputs, but should not be treated as complete physical simulator definitions unless their missing breathing and valvetrain data can be independently documented.
