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
| Inlet-port arrangement | One row completely encircling liner | F/D-family | EMD 710 EMM, Ch.4, Fig. 4-1 / p.4-3 text | Factory 710 family construction; exact dimensions are not tabulated. |

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
| Duracam timing check | 0.66 mm bridge movement at ~110.5° ATDC | F/D-family | EMD 710 EMM, exhaust-valve timing procedure | This is a checking/phasing specification, **not** necessarily zero-lift EVO. |
| Allowed Duracam timing window | ~107.5–112° ATDC | F/D-family | EMD 710 EMM | Service timing tolerance. |
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
| Mechanical turbo drive architecture | Engine gear drive + overrunning clutch + exhaust turbine | F/D-family | EMD 710 EMM | At start/light load engine drives turbo; clutch overruns when turbine torque exceeds mechanical-drive torque. |
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
| 100% | 2600 kW | 191.0 g/kWh | 5.2 kg/s | 340 °C | D declared |
| 85% | 2210 kW | 190.6 g/kWh | 4.6 kg/s | 340 °C | D declared; SFC guaranteed point |
| 75% | 1950 kW | 193.9 g/kWh | 4.2 kg/s | 340 °C | D declared / indicative SFC |
| 50% | 1300 kW | 203.2 g/kWh | 3.0 kg/s | 353 °C | D declared / indicative SFC |

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
