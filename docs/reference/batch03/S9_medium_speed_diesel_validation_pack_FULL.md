# S9 — Medium-speed diesel validation pack

## Scope and grading

Targets selected after bounded Stage-1/Stage-2 screening:

- **Two-stroke:** GM-EMD **16-710G3B**, 45° V16, uniflow-scavenged, mechanically assisted turbocharger.
- **Four-stroke:** Wärtsilä **8L26 AE/DE**, IMO Tier 2, **325 kW/cyl, 900 rpm**, 2600 kW total.

Grades:

- **D** — exact configuration / direct hardware or declared performance value.
- **F** — engine-family, revision-adjacent, or exact family hardware not proven on the locked build.
- **R** — derived from D/F values; arithmetic shown.
- **U** — not found in public sources searched.

Hardware and measured/declared quantities are kept separate from simulator calibration parameters. No silent estimates are used.

---

# 1. GM-EMD 16-710G3B

## 1.1 Configuration lock

Use the RDSO/Indian Railways 16-cylinder 710G3B configuration described at approximately **4500 hp, 954 rpm**, with mechanical unit injectors and EMD mechanically assisted turbocharging. The exact RDSO variant and the factory 710G3B service-manual family are treated separately where revision identity is not proven.

### Primary / near-primary sources

1. **RDSO TS/ED/2012/65 Rev. 2 (July 2014), Annexure 1, Engine Design Data — GM-EMD-710G3B.** Public mirror: https://www.scribd.com/document/618971925/technical-specification-rev-2-july14
2. **EMD 710 Engine Maintenance Manual, E00126ED.** Public mirror: https://www.scribd.com/document/700145515/E00126ED-710-EMM
3. **EMD 710 Databook, 7th Edition** (family technical data; use only as F unless exact variant is shown). Public mirror: https://www.scribd.com/document/768949743/710-Databook-7th-Edition-Technical-Data
4. Ravi, M.R.; Marathe, A.G., **SAE 920782, “Effect of Port Sizes and Timings on the Scavenging Characteristics of a Uniflow Scavenged Engine.”** https://saemobilus.sae.org/papers/effect-port-sizes-timings-scavenging-characteristics-a-uniflow-scavenged-engine-920782

## 1.2 Hardware — geometry

| Item | Value | Grade | Source / location | Notes |
|---|---:|:---:|---|---|
| Bore | 230.19 mm | D | RDSO Annexure 1, p.12 | 9 1/16 in nominal family bore is independently consistent with 710 manual/databook. |
| Stroke | 279.40 mm | D | RDSO Annexure 1, p.12 | 11.0 in. |
| Connecting-rod c-c | 584.2 mm | D | RDSO Annexure 1, p.12 | Use directly in slider-crank geometry. |
| Compression ratio | 16:1 | D | RDSO Annexure 1, p.12 | Exact RDSO configuration. |
| Cylinders | 16 | D | RDSO Annexure 1, p.12 | 45° V. |
| Firing order | 1-8-9-16-3-6-11-14-4-5-12-13-2-7-10-15 | D | RDSO Annexure 1, p.12 | Also consistent with 710 family data. |
| Liner type | Wet | D | RDSO Annexure 1, p.12 | — |
| Piston mass | 27 kg | D | RDSO Annexure 1, p.12 | — |
| Inlet-port arrangement | One row completely encircling liner | F | EMD 710 EMM, Ch.4, Fig. 4-1 / p.4-3 text | Factory 710 family construction; exact dimensions are not tabulated. |

### Derived geometry

- Swept volume per cylinder, **R**:  
  `Vs = π/4 × 0.23019² × 0.27940 = 0.011628 m³ ≈ 11.628 L/cyl`.
- Total displacement, **R**:  
  `16 × 11.628 ≈ 186.05 L`.
- Geometric clearance volume per cylinder, **R**, using CR 16:1:  
  `Vc = Vs/(CR−1) ≈ 11.628/15 ≈ 0.775 L/cyl`.

## 1.3 Gas exchange — ports and valves

| Item | Value | Grade | Source / location | Notes |
|---|---:|:---:|---|---|
| Scavenge-port opening | 45° BBDC | D | RDSO Annexure 1, p.13 | Direct timing value. |
| Scavenge-port closing | 45° ABDC | D | RDSO Annexure 1, p.13 | Direct timing value. |
| Exhaust-valve opening | 103° ATDC | D | RDSO Annexure 1, p.13 | Use as the RDSO event definition. |
| Exhaust valves | 4 / cylinder | D | RDSO Annexure 1, p.13 | Poppet valves in head. |
| Duracam timing check | 0.66 mm bridge movement at ~110.5° ATDC | F | EMD 710 EMM, exhaust-valve timing procedure | This is a checking/phasing specification, **not** necessarily zero-lift EVO. |
| Allowed Duracam timing window | ~107.5–112° ATDC | F | EMD 710 EMM | Service timing tolerance. |
| Exact EVC angle | U | U | — | The retrieved RDSO transcription reports “61° ATDC,” but this conflicts with the factory-described event sequence. Do not use it until the original scan is visually verified. |
| Port height | U | U | — | Not exposed in searched service-data text. |
| Port width | U | U | — | — |
| Port count | U | U | — | Manual confirms a full circumferential row only. |
| Port entry/swirl angle | U | U | — | — |
| Effective area vs crank angle | U | U | — | SAE 920782 is the highest-value source to obtain in full. |
| Exhaust-valve diameter / lift curve | U | U | — | — |

### Important interpretation

The factory manual confirms the sequence **ports close at 45° ABDC and exhaust valves close afterward**. Therefore the transcribed RDSO “EVC 61° ATDC” must not be entered literally. A plausible correction such as 61° ABDC would be an estimate and is intentionally **not** adopted.

## 1.4 Air system / turbocharger

| Item | Value | Grade | Source / location | Notes |
|---|---:|:---:|---|---|
| Turbocharger | GM-EMD 02-J1-1066 | D | RDSO Annexure 1 | Exact RDSO configuration. |
| Aftercoolers | Twin | D | RDSO Annexure 1 | — |
| Turbine entry | Three-entry | D | RDSO Annexure 1 | — |
| Rated turbo speed | 18,150 rpm | D | RDSO Annexure 1 | Rated point. |
| Rated air consumption | 6.0 kg/s | D | RDSO Annexure 1 | Rated point. |
| Ambient pressure | 1.0 bar | D | RDSO Annexure 1 | Preserve source convention. |
| Pressure after compressor / inlet manifold (`PACO`) | 1.25 bar | D | RDSO Annexure 1 | Source pressure convention (absolute vs gauge) is not proven; do not silently reinterpret. |
| Turbine-inlet temperature | 535 °C | D | RDSO Annexure 1 | Rated point; typography in mirror should be checked against original if available. |
| Turbine-outlet temperature | 419 °C | D | RDSO Annexure 1 | Rated point. |
| Mechanical turbo drive architecture | Engine gear drive + overrunning clutch + exhaust turbine | F | EMD 710 EMM | At start/light load engine drives turbo; clutch overruns when turbine torque exceeds mechanical-drive torque. |
| Engine-to-turbo gear ratio | 16.7:1 | F | 16-710G3B-T2 parts-catalogue evidence cited in prior search | Exact G3B-family hardware, but not proven to be the same turbo assembly as the locked RDSO build. |
| Clutch disengagement speed/load | U | U | — | Factory manual describes torque reversal, not a fixed RPM threshold. |
| Compressor map | U | U | — | — |
| Turbine map | U | U | — | — |

### Recommended turbo implementation

Implement the low-load assist as a **one-way/overrunning clutch determined by torque direction**, not by an invented fixed disengagement rpm. Keep the 16.7:1 ratio configurable until exact assembly identity is proven.

## 1.5 Injection system

| Item | Value | Grade | Source | Notes |
|---|---:|:---:|---|---|
| Injection system | Mechanical Unit Injector (MUI) | D | RDSO Annexure 1 | — |
| Supplier | Interstate Diesel, USA | D | RDSO Annexure 1 | As stated. |
| Plunger diameter | 14.2 mm | D | RDSO Annexure 1 | — |
| Nozzle-hole count | 6 | D | RDSO Annexure 1 | — |
| Fuel supply pressure | 6.6 bar | D | RDSO Annexure 1 | — |
| Fuel | HSD / Euro-IV-era Indian diesel specification | D | RDSO Annexure 1 | Use source fuel properties if available; otherwise document substitute. |
| Nozzle-hole diameter | U | U | — | — |
| Included spray angle | U | U | — | — |
| Opening pressure | U | U | — | — |
| Injection-rate trace | U | U | — | — |
| SOI vs notch/load | U | U | — | — |

## 1.6 Thermal / lubrication

| Item | Value | Grade | Notes |
|---|---:|:---:|---|
| Lube-oil inlet pressure | 7.21 bar | D | RDSO rated-point value. |
| Piston-cooling oil pressure | 5.32 bar | D | RDSO rated-point value. |
| Lube-oil cooler outlet temp | 78.5 °C | D | RDSO. |
| Lube-oil cooler inlet temp | 90.1 °C | D | RDSO. |
| Exact coolant heat rejection | U | U | Not found for locked RDSO build. |
| Wall temperatures | U | U | — |
| Motoring/FMEP curve | U | U | — |

## 1.7 Validation outputs

| Quantity | Rated value | Grade | Notes |
|---|---:|:---:|---|
| Speed | 954 rpm | D | RDSO. |
| Power | 4500 hp | D | RDSO. |
| Maximum cylinder pressure | 108 bar | D | RDSO design/performance specification; not a published raw p(θ) trace. |
| Air flow | 6.0 kg/s | D | Same rated point. |
| Turbo speed | 18,150 rpm | D | Same rated point. |
| Turbine inlet temperature | 535 °C | D | Same rated point. |
| Turbine outlet temperature | 419 °C | D | Same rated point. |
| Multi-notch exact-build BSFC | U | U | Public secondary notch/fuel tables were excluded from validation core. |
| Multi-notch exact-build air flow | U | U | — |
| Crank-angle cylinder pressure | U | U | — |
| Motoring pressure / friction | U | U | — |

### Derived rated BMEP

Using 4500 hp = 3355.65 kW, 954 rpm and 186.05 L total displacement for a two-stroke:

`BMEP = P × 60 / (Vd × rpm) ≈ 1.134 MPa = 11.34 bar` **(R)**.

## 1.8 SAE 920782 status

The SAE abstract explicitly says the analyzed geometry has **the same dimensions as the GM EMD 710 engine** and that port/valve sizes and timings were varied. However, the public abstract does **not** expose the numerical baseline port dimensions. Full text remains the best identified source for closing the scavenge-area gap.

No graph was digitised because the actual figures/tables were not publicly accessible in the bounded search.

---

# 2. Wärtsilä 8L26 AE/DE, IMO Tier 2, 325 kW/cyl, 900 rpm

## 2.1 Configuration lock

Use the **8L26 AE/DE, 325 kW/cyl, 900 rpm, 2600 kW**, IMO Tier-2 column of the **Wärtsilä 26 Product Guide a13, 19 September 2018**. Do not mix it with older W26 ratings/camsets except where explicitly labelled **F**.

### Primary / near-primary sources

1. **Wärtsilä 26 Product Guide, a13, 19 September 2018.** Public mirrors include: https://studylib.net/doc/27734778/instruktsiya-po-ekspluatatsii-w-rtsil----w26
2. **W26 Workshop Manual, June 2000** — family/revision data only: https://studylib.net/doc/26087684/wartsila-w26-workshop-manual--1-
3. **Wärtsilä 26 Operation Manual, doc. 26591** — pressure-measurement procedure: https://www.scribd.com/document/942647219/Wartsila-26-26591

## 2.2 Hardware — geometry

| Item | Value | Grade | Source / location | Notes |
|---|---:|:---:|---|---|
| Bore | 260 mm | D | 2018 Product Guide, technical data | — |
| Stroke | 320 mm | D | 2018 Product Guide | — |
| Displacement | 17.0 L/cyl nominal | D | 2018 Product Guide | — |
| Cylinders | 8 inline | D | Locked configuration | — |
| Valves | 2 inlet + 2 exhaust / cyl | D | 2018 Product Guide | — |
| Compression ratio | 16:1 | F | June-2000 Workshop Manual, Ch.1.0 | Do not assume exact for 2018 Tier-2 revision without confirmation. |
| Maximum cylinder pressure | 180 bar | F | June-2000 Workshop Manual | Family design value, not a measured trace for selected build. |
| Connecting-rod length | U | U | — | Not recovered. |
| Firing order | U | U | — | Not recovered for locked modern 8L build. |

### Derived geometry

- Swept volume, **R**: `π/4 × 0.260² × 0.320 = 0.016990 m³ ≈ 16.990 L/cyl`.
- Total, **R**: `8 × 16.990 ≈ 135.92 L`.

## 2.3 Valve train and injection timing

The 2018 guide confirms that the Tier-2 engine may use **Variable Inlet Valve Closure (VIC/VIVC)**, with inlet-valve closing shift up to roughly 30 crank degrees. Therefore old fixed-cam timing must not be treated as D data for the selected 2018 build.

Family-only timing from the June-2000 manual includes later cam pieces 2103ZT141/142 with nominal values around:

| Event | Value | Grade |
|---|---:|:---:|
| Injection checking/reference timing | ~12° BTDC ±4° | F |
| Exhaust opening | ~15.1° BBDC | F |
| Exhaust closing | ~10.0° ATDC | F |
| Inlet opening | ~10.0° BTDC | F |
| Inlet closing | table value ~3.8° before BDC under the manual’s checking convention | F |
| Injection reference | at 4.5 mm pump-plunger lift | F |
| Valve-event checking reference | at 8 mm valve lift | F |

These are useful only as starting family constraints, not exact modern Tier-2 inputs.

Exact 2018 valve-lift curves, VIVC schedule and injection rate/SOI map remain **U**.

## 2.4 Intake / exhaust / turbo system

| Item | Value | Grade | Notes |
|---|---:|:---:|---|
| Turbocharged/intercooled | Yes | D | 2018 Product Guide. |
| 8L gas-exchange arrangement | 2-pulse system | D | Product Guide schematic, Fig. 11-1 in the selected guide revision. |
| Charge-air cooler | One-stage for inline engine | D | Product Guide. |
| Exhaust connection | DN350 class in guide system data | D | Use exact drawing dimensions from guide when constructing external network. |
| Turbo model | U | U | Not disclosed in public guide. |
| Compressor/turbine maps | U | U | — |

## 2.5 Exact multi-load validation table

Locked build: **8L26 AE/DE, 325 kW/cyl, 900 rpm**.

| Load | Power | BSFC | Exhaust mass flow | Exhaust temp after turbo | Grade |
|---:|---:|---:|---:|---:|:---:|
| 100% | 2600 kW | 191.0 g/kWh | 5.2 kg/s | 340 °C | D |
| 85% | 2210 kW | 190.6 g/kWh | 4.6 kg/s | 340 °C | D |
| 75% | 1950 kW | 193.9 g/kWh | 4.2 kg/s | 340 °C | D |
| 50% | 1300 kW | 203.2 g/kWh | 3.0 kg/s | 353 °C | D |

Additional 100% values:

| Quantity | Value | Grade |
|---|---:|:---:|
| Combustion-air flow | 5.1 kg/s | D |
| BMEP | 2.55 MPa | D |
| Nominal charge-air temperature after cooler | 55 °C | D |
| Maximum exhaust backpressure | 5 kPa | D |
| Minimum exhaust pipe diameter | 550 mm | D |

### Heat balance at 100% load

| Path | Heat flow | Grade |
|---|---:|:---:|
| Jacket water | 488 kW | D |
| Charge-air cooler LT circuit | 840 kW | D |
| Lubricating-oil LT circuit | 400 kW | D |
| Radiation | 120 kW | D |

### Fuel-system boundary conditions

| Item | Value | Grade |
|---|---:|:---:|
| Pressure before injection pumps | 700 ± 50 kPa | D |
| Engine-driven MDF pump capacity | 3.7 m³/h | D |
| Approx. circulation flow | 2.2 m³/h | D |
| HFO target viscosity | 16–24 cSt | D |
| MDF minimum viscosity | 2.0 cSt | D |

### Declared tolerances / confidence

The Product Guide states approximately:

- air-flow tolerance: **±5%**
- exhaust-flow tolerance: **±5%**
- exhaust-temperature tolerance: **±20 °C**
- SFC tolerance: **±5%**
- SFC reference LHV: **42.7 MJ/kg**
- **85% SFC is the guaranteed point**; other load points are indicative.

For model validation, weight the 85% BSFC residual most strongly rather than assuming rated load is automatically the highest-confidence consumption point.

## 2.6 Cylinder pressure — hard boundary

The W26 operation manual states that peak pressure measured at the indicator cock can differ from true combustion-space peak by roughly **5–15 bar**, and explicitly warns that the indicator-cock value is mainly suitable for cylinder-to-cylinder comparison. The indicator passage also delays/distorts the pressure pulse.

The manual notes that more sophisticated instruments can produce pressure-vs-crank-angle diagrams, but the bounded public search did **not** locate a numerical p(θ) dataset tied to the locked 2018 8L26 Tier-2 build.

Therefore:

| Quantity | Status |
|---|---|
| Exact p(θ), selected build | **U** |
| True measured pmax, selected build | **U** |
| Old family max/design pressure | **180 bar, F** |
| Indicator-cock readings as true pmax | **Do not use as D** |
| Heat-release trace | **U** |

No pressure curve was digitised because no build-matched public curve was found.

## 2.7 Derived fuel-flow / efficiency checks

Using `m_f = BSFC × P` and LHV = 42.7 MJ/kg:

| Load | Fuel flow | Brake thermal efficiency | Grade |
|---:|---:|---:|:---:|
| 100% | 496.6 kg/h | 44.14% | R |
| 85% | 421.23 kg/h | 44.23% | R |
| 75% | 378.11 kg/h | 43.48% | R |
| 50% | 264.16 kg/h | 41.49% | R |

These are consistency checks, not new measurements.

---

# 3. MODEL-DERIVED / calibration parameters

The following are **not hardware data** and should remain calibration/model parameters unless an exact source is later found:

- discharge coefficients / effective valve and port flow coefficients;
- scavenging efficiency / trapping efficiency correlation coefficients;
- Wiebe or other burn-law parameters;
- wall-temperature fields;
- heat-transfer multipliers;
- friction multipliers / FMEP maps where no motoring data exist;
- turbocharger isentropic-efficiency maps;
- EMD clutch crossover threshold if implemented as anything other than torque-direction logic;
- W26 exact 2018 combustion phasing, VIVC position and injection-rate law.

Do not back-fit these values and then relabel them as hardware.

---

# 4. Recommended simulator use

## 4.1 EMD 16-710G3B

Use as the **two-stroke uniflow / mechanically assisted turbo validation case**.

Strongest constraints:

- exact bore, stroke, rod and CR;
- exact scavenge timing (45° BBDC / 45° ABDC);
- four head exhaust valves and RDSO EVO datum;
- rated air flow, manifold/compressor pressure datum, turbo speed and turbine temperatures;
- rated peak cylinder pressure;
- mechanically driven turbo architecture with overrunning clutch.

Primary blocker before high-confidence scavenging calibration: **actual liner-port geometry / effective area**. Full SAE 920782 remains the highest-value document to obtain.

## 4.2 Wärtsilä 8L26

Use as the **large-bore four-stroke system/thermal validation case**.

Strongest constraints:

- exact 2018 Tier-2 multi-load power/SFC table;
- air and exhaust mass flow;
- exhaust temperature;
- heat rejection split;
- BMEP and gas-path system arrangement.

Do **not** present it as an indicated-combustion validation case until a build-matched p(θ) dataset is obtained.

---

# 5. Remaining high-value gaps

## EMD 710

1. Full SAE 920782 tables/figures or another drawing giving inlet-port height, width/count, tangential angle and area.
2. Original RDSO scan to resolve the corrupted EVC transcription.
3. Exact turbo assembly identity and mechanical-drive ratio for the locked 4500-hp RDSO build.
4. Exact injector-hole diameter/spray angle and injection-rate / SOI data.
5. Multi-load exact-build fuel flow, air flow and p(θ).

## Wärtsilä 8L26

1. Build-matched 2018 Tier-2 compression ratio and rod length.
2. Exact VIVC schedule / valve-lift curves.
3. Exact injection-rate / SOI data.
4. Factory test, commissioning or university test-cell numerical p(θ) data tied to the same engine revision.
5. Friction/motoring data.

These gaps should now be treated as document-acquisition problems, not grounds for continued broad web searching.

---

# 6. Targeted gap-closure extension

## 6.1 Evidence-control rules for this extension

This extension preserves the two previously locked builds and adds ALCO 251 evidence without silently merging configurations:

- **EMD locked build:** Indian Railways/RDSO **16-710G3B, 4500 bhp at 954 rpm**, MUI-equipped.
- **Wärtsilä locked build:** **8L26 AE/DE, IMO Tier 2, high-P6/T6, 325 kW/cyl, 900 rpm**, 2018 Product Guide.
- **ALCO Build A:** conventional Indian Railways **WDM-2 / ALCO 251B V16, nominal 2600 bhp** training/load-box evidence.
- **ALCO Build B:** **RDSO 2009 EFI-retrofit 251B V16** specification. Its EFI pump/timing/notch values are not back-applied to Build A.
- **ALCO family-only evidence:** 251-family nozzle and turbo alternatives, explicitly graded **F**.

### Plot digitisation log

**No plots were digitised in this extension.** Every new numerical value retained below comes from source text or a numerical table. Therefore there are no new `(x, y)` digitised pairs. The SAE 920782 full figures/tables were not publicly recovered, so no port-area curve was inferred from its abstract or thumbnails.

---

## 6.2 EMD 16-710G3B — additions

### 6.2.1 Exact locked-build notch power/speed schedule

A later RDSO functional-requirement specification for retrofitting the existing Indian Railways **EMD 710G3B** fleet reproduces the 4500-bhp engine notch schedule. This is tied to the same 16-cylinder, 4500-bhp-at-954-rpm Indian Railways 710G3B build family as the locked case.

**Source:** RDSO, *Functional Requirement Specification for Up-gradation of Existing HHP Diesel Locomotives to US EPA Tier 0+ Emission Norm by Retro-fitment of Emission Kit on EMD 710G3B Engines*, FRS No. MP.0.08.00.111, Rev.01, May 2023, **Annexure I, pp.9–10**. Public mirror: https://www.scribd.com/document/1047043849/FRS-No-MP-0-08-00-111-1

| Throttle position | Engine speed | Brake power | Grade | Importance | Source |
|---|---:|---:|:---:|:---:|---|
| Low idle | 200 ± 4 rpm | — | D | E | RDSO FRS MP.0.08.00.111 Rev.01, Annexure I, pp.9–10 |
| Idle | 270 ± 15 rpm | — | D | E | same |
| Notch 1 | 270 ± 15 rpm | 259 bhp | D | E | same |
| Notch 2 | 354 ± 15 rpm | 607 bhp | D | E | same |
| Notch 3 | 486 ± 4 rpm | 1148 bhp | D | E | same |
| Notch 4 | 572 ± 4 rpm | 1573 bhp | D | E | same |
| Notch 5 | 675 ± 15 rpm | 1916 bhp | D | E | same |
| Notch 6 | 764 ± 4 rpm | 2960 bhp | D | E | same |
| Notch 7 | 863 ± 15 rpm | 3822 bhp | D | E | same |
| Notch 8 | 954 ± 4 rpm | 4500 bhp | D | E | same |

The same RDSO document identifies the engine as a **16-cylinder, two-stroke, turbocharged 710G3B**, compression ratio **16:1**, bore **230.19 mm**, stroke **279.4 mm**, displacement **11,635 cm³/cyl**, and MUI-equipped, providing an independent configuration cross-check.

### 6.2.2 Locked-build fuel consumption

The 2023 RDSO retrofit specification requires prototype comparison of **SFC and BHP** before/after retrofit, but the publicly accessible specification does **not publish the measured SFC values**.

| Required quantity | Value | Grade | Importance | Source / result |
|---|---:|:---:|:---:|---|
| Rated 4500-bhp fuel flow / BSFC | U | U | E | No numeric value in RDSO 2014 design-data annexure or 2023 retrofit FRS searched |
| Notch 1–8 exact-build fuel flow / BSFC | U | U | E | Same |
| Notch 1–8 exact-build paired fuel/power/speed acceptance table | U | U | E | Requirement exists, results not public in located RDSO documents |

Therefore the exact-build **power/speed schedule is now D**, but the exact-build **fuel consumption remains an unresolved essential gap**.

### 6.2.3 Family measured multi-load boost / air-box conditions — keep separate

A field-emissions study contains measured operating telemetry for locomotive **NC1893**, fitted with an **EMD 12-710G3B**, rated **3000 hp at 900 rpm**. This is **not** the locked 16-cylinder RDSO engine and is retained only as **F** evidence for 710G3B-family boost behaviour.

**Source:** *Demonstration of Alternative Methodology of Measuring Emissions of Locomotive Engines*, Appendix C, **Table C-29 (p.92), Table C-30 (pp.92–93)**. Public copy: https://www.researchgate.net/publication/385749614_Demonstration_of_Alternative_Methodology_of_Measuring_Emissions_of_Locomotive_Engines

The report states that RPM, manifold absolute pressure (MAP) and intake-air temperature were measured by the PEMS instrumentation; IAT was measured in the air box. `MAP` is reported in kPa and is treated here as **absolute pressure by the source variable definition**, not as gauge boost.

| Mode | Speed (rpm) | Recorded power (hp) | MAP (kPa abs) | Air-box IAT (°C) | Grade |
|---|---:|---:|---:|---:|:---:|
| Low idle | 201 | 9 | 103 | 68 | F |
| High idle | 350 | 9 | 111 | 74 | F |
| Notch 1 | 350 | 190 | 111 | 70 | F |
| Notch 2 | 350 | 350 | 112 | 73 | F |
| Notch 3 | 492 | 675 | 125 | 75 | F |
| Notch 4 | 570 | 1000 | 136 | 75 | F |
| Notch 5 | 653 | 1325 | 148 | 75 | F |
| Notch 6 | 732 | 1600 | 166 | 76 | F |
| Notch 7 | 827 | 2400 | 221 | 79 | F |
| Notch 8 | 909 | 2700 | 254 | 82 | F |

This is useful for checking the **shape** of 710-family boost rise, but it must not be used as D boundary data for the 16-710G3B RDSO build.

#### Source-derived fuel-rate table — MODEL-DERIVED IN THE SOURCE

The same report gives the following 12-710G3B fuel-use rates in **Table C-31, p.93**:

| Mode | Speed (rpm) | Power (hp) | Reported fuel rate (g/s) | Grade |
|---|---:|---:|---:|:---:|
| Low idle | 201 | 9 | 2.95 | F |
| High idle | 350 | 9 | 6.58 | F |
| Notch 1 | 350 | 190 | 10.2 | F |
| Notch 2 | 350 | 350 | 16.7 | F |
| Notch 3 | 492 | 675 | 30.6 | F |
| Notch 4 | 570 | 1000 | 44.5 | F |
| Notch 5 | 653 | 1325 | 58.7 | F |
| Notch 6 | 732 | 1600 | 72.1 | F |
| Notch 7 | 827 | 2400 | 106 | F |
| Notch 8 | 909 | 2700 | 121 | F |

**Critical qualification:** the report states that the fuel-use rate was based on calibration of engine volumetric efficiency so that BSFC matched a separate locomotive dynamometer measurement. These are therefore **not direct fuel-flow measurements** and are intentionally excluded from the locked-build hardware/output table.

### 6.2.4 Exhaust-valve closing — family cross-check, exact RDSO scan still unresolved

Indian Railways training material gives a WDG4/GM two-stroke timing diagram with:

- exhaust valves open: **109° ATDC**;
- scavenge ports open: **43.5° BBDC**;
- scavenge ports close: **43.5° ABDC**;
- exhaust valves close: approximately **67° ABDC**;
- fuel injection begins: approximately **15.8° BTDC**.

**Source:** Indian Railways / IRIMEE, *Diesel Engine Fundamentals and Working Principles*, **pp.7–8, “Valve Timing — Two Stroke Cycle”, Sketch 1.3**. Official-file URL: https://irimee.indianrailways.gov.in/instt/uploads/files/1477552111913-Fundamentals_of_Diesel_Engines.pdf ; searchable mirror: https://studylib.net/doc/25691032/1477552111913-fundamentals-of-diesel-engines

These values are **F family evidence**, because they differ slightly from the locked RDSO build's 103° ATDC EVO and 45° BBDC/ABDC port timing. They strongly confirm that exhaust-valve closure belongs **after BDC**, but they do **not** prove that the corrupted locked-build RDSO value should be “61° ABDC”.

| Item | Locked-build result | Grade | Importance |
|---|---:|:---:|:---:|
| Original RDSO Annexure-1 EVC scan reading | U | U | E |
| Family WDG4/GM EVC cross-check | ~67° ABDC | F | E |
| Exhaust-valve head diameter | U | U | E |
| Full valve-lift curve | U | U | E |
| Duracam phasing/check datum | 0.66 mm bridge movement at ~110.5° ATDC | F | E |

### 6.2.5 Turbo mechanical drive — source strengthened

The previously retained **16.7:1** ratio can now be tied to a specific 16-710G3B-family parts catalogue entry:

**Source:** Electro-Motive Diesel, Inc., *16-710G3B-T2 MPI/Go-Transit Service Parts Catalog*, Catalog No. E108 LXO, Order No. 20099243, Aug. 2010, **Turbocharger Application, p.1-85**. Public mirror: https://pdfcoffee.com/16-710g3b-t2-parts-catalog-pdf-free.html

The catalog lists turbocharger assembly **P/N 40120139** and states **“Engine – 16.7:1 gear ratio.”**

This remains **F** for the locked RDSO build because no public cross-reference was found proving that RDSO's named **GM-EMD 02-J1-1066** is assembly P/N 40120139.

| Turbo item | Result | Grade | Importance |
|---|---:|:---:|:---:|
| Locked RDSO turbo identity | GM-EMD 02-J1-1066 | D | I |
| 16-710G3B-T2 parts-catalog assembly | P/N 40120139 | F | I |
| Gear ratio for P/N 40120139 application | 16.7:1 engine:turbo | F | I |
| Exact cross-reference 02-J1-1066 ↔ P/N 40120139 | U | U | I |
| Compressor map | U | U | I |
| Turbine map | U | U | I |
| Numerical clutch takeover load/speed | U | U | I |
| More-than-one-load locked-build TIT | U | U | I |
| More-than-one-load locked-build air flow | U | U | I |

The EMD factory manual's qualitative description remains the best clutch evidence: the engine drives the turbo at starting/light load; as exhaust-gas turbine torque becomes sufficient, torque reverses in the planetary drive and the overrunning clutch releases. No public numeric crossover threshold was recovered.

### 6.2.6 Injection, cylinder pressure and friction — targeted result

No validation-grade locked-build values were recovered for:

| Quantity | Result | Grade | Importance |
|---|---:|:---:|:---:|
| MUI nozzle-hole diameter | U | U | I |
| MUI included spray angle | U | U | I |
| MUI opening pressure | U | U | I |
| MUI SOI vs notch | U | U | I |
| Locked-build pmax at more than one load | U | U | I |
| Measured 710 crank-angle cylinder-pressure trace | U | U | N |
| Measured 645 crank-angle cylinder-pressure trace suitable for numerical validation | U | U | N |
| Motoring/FMEP curve | U | U | N |

The RDSO EUI development specification contains prospective EUI design targets, but those belong to a different injection-system build and are not mixed into this MUI pack.

---

## 6.3 Wärtsilä 8L26 AE/DE Tier 2 — additions

### 6.3.1 Source-control upgrade for the locked 2018 build

For the locked build, use the manufacturer document directly where available:

**Wärtsilä 26 Product Guide, a13, 19 September 2018.** Public PDF: https://www.amasenergy.com/sites/default/files/products/amas_import/dieselengine_11453_1594749449_1594749449.pdf

Key exact locations used in this extension:

- **p.3-19:** 8L26 high-P6/T6 technical-data table containing the selected 325 kW/cyl, 900 rpm column.
- **p.4-3:** Variable Inlet valve Closure (VIC) description.
- **p.11-1, Fig. 11-1:** 8L charge-air / exhaust-gas 2-pulse system and measurement-point identifiers.

The earlier StudyLib link remains useful as a mirror, but the manufacturer PDF above is the preferred source reference for these values.

### 6.3.2 Compression ratio and connecting rod — exact-build status

The targeted search did **not** recover a 2018-build-specific compression ratio or connecting-rod centre-to-centre length.

The June-2000 workshop manual contains a connecting-rod inspection/reference dimension of **448.898–449.025 mm** from the small-end reference to the big-end steel bore without bearings (**manual p.134**). This is **not explicitly stated as centre-to-centre rod length**, so it must not be substituted for the requested slider-crank dimension.

The same older manual documents piston/revision changes including compression ratios **13.5:1** and **15.8:1** in a technical-notice section, while general family data elsewhere state **16:1**. The coexistence of these revisions is direct evidence that the modern 2018 value cannot safely be inherited from the old family manual.

| Quantity | Value | Grade | Importance | Source |
|---|---:|:---:|:---:|---|
| Exact 2018 Tier-2 compression ratio | U | U | E | Not stated in locked 2018 Product Guide |
| Old W26 general-data compression ratio | 16:1 | F | E | June-2000 Workshop Manual, Ch.1.0 |
| Older piston-revision compression ratios | 13.5:1 and 15.8:1 | F | E | June-2000 Workshop Manual, Ch.2.8 technical notice |
| Rod inspection/reference dimension | 448.898–449.025 mm | F | E | June-2000 Workshop Manual, p.134 |
| Exact 2018 rod centre-to-centre length | U | U | E | Not recovered |

### 6.3.3 Charge-air pressure and turbine-inlet temperature at the four declared loads

The locked 2018 table gives air/exhaust flow and post-turbo temperature, but it does **not** give numerical charge-air pressure or turbine-inlet temperature at the 100/85/75/50% points.

Fig. 11-1 confirms that the engine/system has the relevant instrumentation:

- **TE511:** exhaust-gas temperature at turbocharger inlet;
- **TE517:** exhaust-gas temperature at turbocharger outlet;
- **SE518:** turbocharger speed;
- **PI/PT601:** charge-air pressure at engine inlet.

**Source:** 2018 Product Guide, **p.11-1, Fig.11-1**.

| Load | Charge-air pressure | TIT / TE511 | Grade | Importance |
|---:|---:|---:|:---:|:---:|
| 100% | U | U | U | E |
| 85% | U | U | U | E |
| 75% | U | U | U | E |
| 50% | U | U | U | E |

No values were back-calculated from mass flow because doing so would require assumptions about compressor efficiency, manifold temperature distribution and pressure losses.

### 6.3.4 VIC / valve timing

The 2018 Product Guide states that **Variable Inlet valve Closure (VIC)** is available on IMO Tier-2 engines. It describes:

- earlier inlet-valve closing at high load;
- advance returning to **zero at low load**;
- maximum adjustable advance of approximately **30° crank angle**.

**Source:** 2018 Product Guide, **p.4-3**.

| Item | Value | Grade | Importance |
|---|---:|:---:|:---:|
| VIC capability | present on IMO Tier-2 W26 | D | I |
| Maximum IVC advance | up to 30°CA | D | I |
| Low-load advance | 0°CA | D | I |
| Numerical VIC schedule vs 100/85/75/50% load | U | U | I |
| Exact 2018 IVO/EVO/EVC values | U | U | I |
| Exact 2018 valve-lift curves | U | U | I |

#### Older camset checking points — family only

The June-2000 workshop manual provides tabulated checking points for cam pieces **2103ZT141/142**. These are **F**, not 2018-build timing. The following are **transcribed table values, not plot digitisation**:

| Injection timing reference (°BTDC) | Exhaust opens at 5.6 mm lift (°BBDC) | Exhaust closes at 5.6 mm lift (°ATDC) | Inlet opens at 4.1 mm lift (°BTDC) | Inlet closes at 4.1 mm lift (°BTDC, manual convention) | Grade |
|---:|---:|---:|---:|---:|:---:|
| 13.5 | 23.5 | 19.9 | 31.5 | 6.3 | F |
| 13.0 | 23.0 | 20.4 | 31.0 | 6.8 | F |
| 12.5 | 22.5 | 20.9 | 30.5 | 7.3 | F |
| 12.0 | 22.0 | 21.4 | 30.0 | 7.8 | F |
| 11.5 | 21.5 | 21.9 | 29.5 | 8.3 | F |
| 11.0 | 21.0 | 22.4 | 29.0 | 8.8 | F |
| 10.5 | 20.5 | 22.9 | 28.5 | 9.3 | F |
| 10.0 | 20.0 | 23.4 | 28.0 | 9.8 | F |

**Source:** Wärtsilä W26 Workshop Manual, June 2000, Ch.2 camshaft technical-notice tables associated with 2103ZT141/142. These points are retained only to constrain plausible family valve motion if no modern drawing is available.

### 6.3.5 Injection system — modern hardware capability

The 2018 Product Guide states that each cylinder has an **individual high-pressure, flow-through mono-element fuel-injection pump** and that the system is designed for injection pressures **up to 1500 bar**.

**Source:** 2018 Product Guide, **p.4-4** (fuel-injection-system description immediately following the VIC section).

| Injection item | Value | Grade | Importance |
|---|---:|:---:|:---:|
| High-pressure pump arrangement | 1 individual mono-element pump/cylinder | D | I |
| Design injection-pressure capability | up to 1500 bar | D | I |
| Low-pressure supply before pumps | 700 ± 50 kPa | D | I |
| Exact pump/injection pressure at 100/85/75/50% load | U | U | I |
| Exact SOI at 100/85/75/50% load | U | U | I |
| Injection-rate trace | U | U | I |

The **1500 bar** value is a design capability, not evidence that the selected operating points all inject at 1500 bar.

### 6.3.6 Cylinder pressure, friction and firing order

No build-matched numerical W26 pressure trace was recovered in the targeted pass.

| Quantity | Result | Grade | Importance |
|---|---:|:---:|:---:|
| Measured p(θ), any operating point on locked 2018 8L26 build | U | U | I |
| Measured p(θ), other W26 build with sufficiently documented operating point | U | U | I |
| Mechanical efficiency / FMEP map | U | U | N |
| Motoring curve | U | U | N |
| 8L26 firing order for locked build | U | U | N |

The previously documented indicator-cock warning remains applicable: do not treat an indicator-cock peak as an undistorted combustion-space pmax.

---

# 6.4 ALCO 251 — newly covered

The original Stage-1 search named ALCO 251 but the previous pack did not extract it. Public evidence is useful but fragmented. To avoid configuration mixing, it is split below into a conventional Indian Railways WDM-2/251B layer, a separate RDSO EFI-retrofit build, and family-only nozzle/turbo evidence.

## 6.4.1 ALCO Build A — conventional Indian Railways WDM-2 / 251B V16

### Sources

1. Indian Railways / IRIMEE, *Diesel Engine Fundamentals and Working Principles*. Official-file URL: https://irimee.indianrailways.gov.in/instt/uploads/files/1477552111913-Fundamentals_of_Diesel_Engines.pdf ; searchable mirror: https://studylib.net/doc/25691032/1477552111913-fundamentals-of-diesel-engines
   - **pp.8–9, Sketch 1.4:** ALCO four-stroke valve timing.
   - **pp.17–18:** WDM-2 / ALCO 251B principal engine data.
2. IRIMEE, *Load Box Testing, HP, SFC of Diesel Loco* (training material, Dec. 2019). Public mirror: https://www.scribd.com/document/802772222/Alco-Load-Box-Testing-HP-SFC-1
   - worked **WDM-2 2600 HP SFC example**;
   - table headed **“Load Box Parameters”**;
   - turbo/exhaust-temperature notes and cylinder firing-pressure example.
   - The document itself warns that its example parameter table is for calculation/explanation and is **not a standard acceptance table**, so these data are graded **F**.
3. RDSO EFI specification is treated separately in §6.4.2 and is **not** used to redefine this conventional build.

### Geometry / basic configuration

| Item | Value | Grade | Importance | Source |
|---|---:|:---:|:---:|---|
| Engine | ALCO 251B | F | E | IRIMEE Fundamentals, pp.17–18 |
| Cylinders | 16 V | F | E | same |
| Cycle | four-stroke | F | E | same |
| Bore | 228 mm nominal | F | E | IRIMEE Fundamentals, pp.17–18 |
| Bore, RDSO EFI-spec nominal | 228.6 mm (9 in) | D | E | see §6.4.2 |
| Stroke | 267 mm nominal | F | E | IRIMEE Fundamentals, pp.17–18 |
| Stroke, RDSO EFI-spec nominal | 266.7 mm (10.5 in) | D | E | see §6.4.2 |
| Compression ratio, concave-crown piston | 12.5:1 | F | E | IRIMEE Fundamentals, pp.17–18 |
| Compression ratio, flat-crown piston | 11.5:1 | F | E | same |
| Connecting-rod centre-to-centre length | U | U | E | Not recovered |
| Nominal swept volume | 10.5 L/cyl | F | E | IRIMEE Fundamentals, pp.17–18 |
| Turbo-supercharged with charge-air cooling | yes | F | E | IRIMEE Fundamentals, pp.17–18 |
| Gross rating | 2600 bhp at 1000 rpm | F | E | IRIMEE Fundamentals, pp.17–18 |
| Idle speed | 400 rpm | F | E | same |
| BMEP | 13.57 kgf/cm² | F | E | same |

The two listed compression ratios correspond to different piston configurations; they must not be averaged or silently assigned to an unspecified WDM-2 engine.

### Valve timing — two WDM-2 cam/timing variants

IRIMEE Sketch 1.4 presents conventional and “fuel-efficient” ALCO timing values. They are retained as **separate F alternatives**:

| Event | Conventional timing | Fuel-efficient timing | Grade | Source |
|---|---:|---:|:---:|---|
| Inlet opens | 63°48′ BTDC | 80.1° BTDC | F | IRIMEE Fundamentals, pp.8–9, Sketch 1.4 |
| Inlet closes | 29°40′ ABDC | 35.4° ABDC | F | same |
| Exhaust opens | 41°28′ BBDC | 57.67° BBDC | F | same |
| Exhaust closes | 60° ATDC | 57.77° ATDC | F | same |
| Valve overlap | 123°48′ | 137.87° | F | same |

The same training text gives an example ALCO/WDM-2 fuel-injection start of approximately **18.25° BTDC** (**F**). It is a family/training value, not an exact pump setting for a uniquely identified locomotive.

### Charge-air pressure / turbo

The IRIMEE explanation states that during overlap the supercharged air is about **1.5 kgf/cm² above atmospheric pressure**.

- Source value: **1.5 kgf/cm² gauge**, **F**.
- Derived conversion: `1.5 × 0.980665 = 1.471 bar(g)`, **R**.

This is a family/training datum rather than a full-load acceptance measurement tied to a serial-numbered engine.

The load-box training material lists several WDM-2 high-efficiency turbocharger alternatives rather than one universal build:

| Turbocharger | Stated maximum speed | Grade | Source |
|---|---:|:---:|---|
| ABB VTC-304-VG15 | 24,600 rpm | F | IRIMEE Load Box guide, turbo notes |
| Napier NA-295IR | 27,000 rpm | F | same |
| GE 7S1716 | 20,000 rpm | F | same |

Therefore **exact turbo identity for the selected conventional 251B build remains U** unless a locomotive/engine build record identifies which alternative is fitted.

### Performance / fuel consumption

IRIMEE's worked WDM-2 load-box example gives:

- generator voltage: **568 V**;
- generator current: **2901 A**;
- assumed generator efficiency: **0.935**;
- calculated traction-generator output: **2208.80 hp**;
- calculated input to traction generator: **2362.35 hp**;
- correction factor: **0.98**;
- auxiliary horsepower: **186 hp**;
- calculated engine brake power: **2596 hp**;
- fuel quantity: **10,000 g** consumed in **88 s**;
- calculated SFC: **157.58 g/hp·h**.

**Source:** IRIMEE, *Load Box Testing, HP, SFC of Diesel Loco*, worked **“HP, SFC” / WDM2 2600HP** example.

Derived SI checks:

- `2596 hp × 0.7456999 = 1935.84 kW`, **R**.
- `10 kg / 88 s × 3600 = 409.09 kg/h`, **R**.
- `157.58 g/(hp·h) ÷ 0.7456999 = 211.32 g/kWh`, **R**.

These are **training/example data**, not a serial-numbered acceptance run, so the source values are **F**.

The same guide's illustrative “Load Box Parameters” table gives the following speed schedule and explicitly warns that the table is **not standard**:

| Notch | Speed (rpm) | Grade |
|---:|---:|:---:|
| 1 | 400 | F |
| 2 | 490 | F |
| 3 | 590 | F |
| 4 | 690 | F |
| 5 | 740 | F |
| 6 | 800 | F |
| 7 | 900 | F |
| 8 | 1000 | F |

Do not merge this conventional training schedule with the separate EFI-retrofit notch schedule in §6.4.2.

### Exhaust temperature / peak cylinder pressure

The load-box guide gives WDM-2 high-efficiency-turbo guidance including:

- turbine-inlet temperature maximum: **620 °C**, **F**;
- exhaust-gas temperature: approximately **490–510 °C** in the referenced 720-A turbo context, **F**.

Its illustrative cylinder table contains firing pressures spanning approximately **1250–1550 psi**, corresponding to:

- `1250 psi × 0.0689476 = 86.18 bar`, **R**;
- `1550 psi × 0.0689476 = 106.87 bar`, **R**.

The source explicitly treats the table as an explanatory example; these values therefore **must not be presented as a D-rated 251B pmax map**.

### Conventional-build unresolved fields

| Quantity | Result | Grade | Importance |
|---|---:|:---:|:---:|
| Connecting-rod length | U | U | E |
| Exact piston/CR for one identified 16-cyl locomotive | U | U | E |
| Exact conventional pump model/plunger for one identified build | U | U | E |
| Exact nozzle holes × diameter for one identified build | U | U | E |
| Exact turbo model for one identified build | U | U | E |
| Measured air flow vs notch | U | U | E |
| Measured boost vs notch | U | U | E |
| Acceptance-grade SFC vs notch | U | U | E |
| Acceptance-grade exhaust temperature vs notch | U | U | E |
| Acceptance-grade pmax vs notch | U | U | E |
| Numerical p(θ) | U | U | N |

---

## 6.4.2 ALCO Build B — RDSO 2009 EFI-retrofit 251B V16

This is a **different build** from the conventional WDM-2 layer above.

**Source:** Government of India / RDSO, *Specification for Electronic Fuel Injection (EFI) System for ALCo Locomotives*, **MP.0.08.00.91, Rev.00, Dec. 2009**, especially **§2 “Details of ALCO 251-B Engine” and §4.2 notch-RPM table**. Public mirror: https://www.scribd.com/document/791867829/MP-0-0800-91-Dec-09

| Item | Value | Grade | Importance | Notes |
|---|---:|:---:|:---:|---|
| Engine | ALCO 251-B V16 | D | E | EFI-retrofit specification |
| Cycle | 4-stroke, turbocharged | D | E | same |
| Bore | 228.6 mm (9 in) | D | E | §2 |
| Stroke | 266.7 mm (10.5 in) | D | E | §2 |
| EFI pump | Bosch B416810385 | D | E | One pump/cylinder |
| Number of EFI pumps | 16 | D | E | — |
| Pump type | Bosch jerk-type, solenoid spill/control, cam-driven | D | E | Existing FIP cam-lobe drive retained |
| Fuel-cam part number | DLW P/N 10031364 | D | E | Stiffer fuel-cam profile stated |
| Approx. full-load injection actuation window | 22° BTDC to 12° ATDC | D | E | Preserve source wording; do not reinterpret as a single SOI value |
| Firing order | 1R-1L-4R-4L-7R-7L-6R-6L-8R-8L-5R-5L-2R-2L-3R-3L | D | E | §2 |

EFI-retrofit notch schedule:

| Notch | Engine speed (rpm) | Grade | Source |
|---:|---:|:---:|---|
| 1 | 350 | D | RDSO MP.0.08.00.91 Rev.00, §4.2 |
| 2 | 450 | D | same |
| 3 | 550 | D | same |
| 4 | 650 | D | same |
| 5 | 750 | D | same |
| 6 | 850 | D | same |
| 7 | 950 | D | same |
| 8 | 1050 | D | same |

No exact EFI nozzle-hole diameter, spray angle, air-flow map, boost map, exhaust-temperature map or pmax map was recovered from this specification in the bounded pass.

---

## 6.4.3 ALCO 251 family-only nozzle evidence

A commercial ALCO-power nozzle catalogue lists several **251-family alternatives**. Because no source ties one of them to the specific conventional WDM-2 or RDSO EFI build above, all are **F** and must remain alternatives rather than a selected nozzle.

**Source:** *Nozzle Manual / ALCO POWER* catalogue, 251 application table, **p.2**. Public mirror: https://www.scribd.com/document/1029009286/Nozzle-Manual

| OEM / catalogue number | Holes × diameter | Included spray angle | Grade |
|---|---:|---:|:---:|
| 304010566 | 9 × 0.35 mm | 145° | F |
| 304010570 | 9 × 0.40 mm | 145° | F |
| 304010580 | 9 × 0.325 mm | 145° | F |
| 304010585 | 9 × 0.375 mm | 160° | F |
| 304030341 | 9 × 0.35 mm | 145° | F |
| 304030343 | 8 × 0.38 mm | 145° | F |

The same catalogue lists 251-family pump-element/plunger alternatives including **16.00 mm** and **15.00 mm** diameters. These are **F** only; an exact build assignment was not recovered.

---

# 6.5 Updated coverage / decision matrix

| Requirement | EMD 16-710G3B locked RDSO | Wärtsilä 8L26 locked 2018 | ALCO 251B/C public evidence |
|---|:---:|:---:|:---:|
| Bore / stroke | D | D | D/F depending build |
| Rod length | D | U exact / F reference dimension | U |
| Compression ratio | D | U exact / F family revisions | F alternatives |
| Full valve / port event definition | D/P, EVC exact unresolved | U exact / F family tables | F conventional/fuel-efficient variants |
| Scavenge-port dimensions / area | U | n/a | n/a |
| Injection hardware | D MUI | D pump architecture | D EFI / F family nozzle alternatives |
| Exact nozzle geometry | U | U | U exact; F family alternatives |
| Multi-load power/speed | D | D | F conventional / D EFI speed schedule |
| Exact-build BSFC/fuel flow | U | D | F worked example only |
| Multi-load air flow | U | D exhaust, 100% combustion air | U |
| Multi-load boost | U exact; F 12-710G3B MAP | U | U exact |
| Multi-load TIT | U exact | U | U exact |
| Rated/illustrative pmax | D rated 108 bar | F family 180 bar max | F illustrative range |
| Numerical p(θ) | U | U | U |
| Friction / motoring | U | U | U |
| Turbo map | U | U | U |

### Updated recommended use

- **EMD 16-710G3B:** remains the preferred two-stroke/uniflow/mechanically-assisted-turbo case. The exact notch **power/speed** schedule is now available, but **fuel consumption and port geometry remain the two essential blockers** for a high-confidence multi-load validation.
- **Wärtsilä 8L26:** remains the preferred large-bore four-stroke **thermal/system** case. The 2018 VIC and 1500-bar pump capability are now documented, but exact modern CR/rod, four-load boost/TIT and p(θ) remain unavailable.
- **ALCO 251B:** useful as a **secondary four-stroke locomotive case**, especially for valve-timing variants, load-box/SFC procedures and EFI architecture. It is not yet as internally coherent as the W26 because public geometry, nozzle, turbo and measured-output data come from different explicitly separated build layers.

---

# 7. MODEL-DERIVED additions from this extension

Keep these separate from hardware/measurements:

1. **ALCO WDM-2 boost conversion:** `1.5 kgf/cm² × 0.980665 = 1.471 bar(g)` — **R**.
2. **ALCO load-box example power conversion:** `2596 hp × 0.7456999 = 1935.84 kW` — **R**.
3. **ALCO load-box example fuel flow:** `10 kg / 88 s × 3600 = 409.09 kg/h` — **R**.
4. **ALCO load-box example SI SFC:** `157.58 g/(hp·h) ÷ 0.7456999 = 211.32 g/kWh` — **R**.
5. **ALCO illustrative firing-pressure conversion:** `1250–1550 psi × 0.0689476 = 86.18–106.87 bar` — **R**.
6. The **12-710G3B NC1893 fuel-rate table** is explicitly labelled **MODEL-DERIVED in its source** because the report calibrated volumetric efficiency to match another locomotive's dynamometer BSFC; do not treat those g/s values as direct fuel-flow measurements.

No other new simulator calibration constants were inferred.

---

# 8. Remaining document-acquisition targets after this extension

These are now the only missing documents likely to change implementation materially:

### EMD 16-710G3B
1. Full **SAE 920782** or an EMD liner drawing with port count, width, height, angles and baseline effective area.
2. The **original RDSO Annexure 1 scan/image** resolving the EVC field.
3. A locked-build **acceptance/type-test sheet** with fuel rate/BSFC, air flow, pmax and turbine temperatures by notch.
4. Exact **02-J1-1066** turbo parts/application cross-reference and, ideally, compressor/turbine maps.
5. MUI nozzle drawing/specification.

### Wärtsilä 8L26
1. 2018-build **engine data sheet / piston-and-rod drawing** confirming CR and rod c-c.
2. Factory test/commissioning sheet containing **PI/PT601 charge pressure and TE511 TIT** at the declared loads.
3. Modern Tier-2 **VIC schedule and cam/lift data**.
4. Build-matched injection timing/pressure schedule.
5. Indicating dataset with numerical p(θ).

### ALCO 251
1. A specific **MI-series engine manual** or DLW build sheet tying rod length, piston/CR, pump and nozzle to one 16-cylinder locomotive build.
2. A corresponding turbocharger application record and acceptance/load-box sheet with measured air flow, boost, EGT and pmax.
3. Multi-notch fuel-consumption data from an actual acceptance/type test rather than the training calculation example.

Further generic searching is unlikely to improve the pack without one of those specific documents.
