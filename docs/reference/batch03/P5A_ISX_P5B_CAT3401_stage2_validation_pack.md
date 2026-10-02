# P5 replacement — paired heavy-duty validation pack, Stage 2

## Configuration rule

This pack deliberately uses **two separate engines**.

- **P5A — Cummins ISX 15 L, DOE/Cummins five-mode dataset**: full-engine brake, fresh-air, fuel, EGR and gas-exchange boundary validation.
- **P5B — Caterpillar 3401 NRC single-cylinder, Dev et al. 2020 high-load build**: large-bore combustion, heat release, injection and thermal-boundary validation.

**Do not merge their hardware or operating points.**

Grades:
- **D** = exact selected experimental configuration / directly reported measurement.
- **F** = family / related build.
- **R** = derived; arithmetic stated.
- **U** = not found.
- **MODEL-DERIVED** = simulation-only quantity; not hardware.

---

# P5A — Cummins ISX 15 L, DOE/Cummins five-mode build

## Source lock

Primary report: *Thermoelectric Conversion of Waste Heat to Electricity in an IC Engine Powered Vehicle*, DOE grant FC26-04NT42281, DOI 10.2172/1045212.

- Primary OSTI PDF: https://www.osti.gov/servlets/purl/1045212
- UNT record: https://digital.library.unt.edu/ark:/67531/metadc832102/
- Searchable mirror used to verify Table 2.1 text: https://studylib.net/doc/18530846/thermoelectric-conversion-of-waste-heat-to-electricity-in...

The report describes the modeled/tested engine as a **direct-injection, turbocharged inline-six Cummins ISX**, bore **137 mm**, stroke **169 mm**, displacement **14.95 L**, with **VGT, intercooler and EGR cooler**. These are D for this DOE ISX study. The exact commercial rating is not stated in the extracted section and is therefore not imported from later ISX15 rating sheets.

## HARDWARE

| Item | Value | Grade | Source |
|---|---:|:---:|---|
| Cylinders | 6, inline | D | DOE report §2.2.1 |
| Bore | 137 mm | D | DOE report §2.2.1 |
| Stroke | 169 mm | D | DOE report §2.2.1 |
| Displacement | 14.95 L | D | DOE report §2.2.1 |
| Direct injection | present | D | DOE report §2.2.1 |
| Variable-geometry turbocharger | present | D | DOE report §2.2.1 |
| Intercooler | present | D | DOE report §2.2.1 |
| EGR cooler | present | D | DOE report §2.2.1 |
| Compression ratio | 17:1 | F | NREL 2004 ISX 450/1650 production-development engine; not proven same DOE build |
| Rod length | U | U | not found |
| Bowl geometry | U | U | not found |
| Valve timing/lift | U | U | not found |
| Injector holes × diameter | U | U | not found |
| Pointwise injection pressure/SOI/quantity | U | U | not found |
| Compressor/turbine maps | U | U | not found |
| Turbo shaft speed | U | U | not found |

Family source for CR only: https://afdc.energy.gov/files/pdfs/35911.pdf, p.4.

## Exact five-mode experimental table

Source: DOE report, **Table 2.1, pp. 13–14**. The report says these are “experimentally determined values” provided by Cummins for five primary operating modes approximating ESC conditions.

| Point | rpm | Torque Nm | BMEP bar | Brake kW | Fuel kg/h | Fresh air kg/h | BSFC g/kWh R | AFR R | EGR kg/h | EGR frac | Intake p bar abs | Intake T K | Exhaust-manifold p bar abs | Exhaust-manifold T K |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| A-25 | 1230 | 640.1 | 5.38 | 82.46 | 18.28 | 539.1 | 221.7 | 29.49 | 195.4 | 0.27 | 1.47 | 322.08 | 1.62 | 662.74 |
| A-100 | 1230 | 2558.2 | 21.51 | 329.52 | 67.02 | 1300.4 | 203.4 | 19.40 | 268.6 | 0.17 | 3.07 | 323.03 | 3.38 | 905.87 |
| B-62 | 1500 | 1586.6 | 13.34 | 249.23 | 50.75 | 1271.2 | 203.6 | 25.05 | 348.4 | 0.22 | 2.56 | 318.94 | 2.83 | 780.52 |
| B-100 | 1500 | 2558.8 | 21.51 | 401.96 | 83.26 | 1729.5 | 207.1 | 20.77 | 434.4 | 0.20 | 3.49 | 327.86 | 3.91 | 899.59 |
| C-100 | 1800 | 2139.1 | 17.98 | 403.22 | 89.20 | 1862.1 | 221.2 | 20.88 | 359.8 | 0.16 | 3.11 | 330.44 | 3.70 | 940.71 |

### Derivations

- `torque_Nm = torque_ft-lb × 1.35581795`
- `BMEP_bar = BMEP_psi × 0.06894757`
- `fresh_air_kg/h = air_lb/min × 0.45359237 × 60`
- `EGR_kg/h = EGR_lb/min × 0.45359237 × 60`
- `BSFC = fuel_kg/h × 1000 / brake_kW`
- `AFR = fresh_air_kg/h / fuel_kg/h`

The report itself prints fuel-air ratio and EGR fraction; the derived AFRs cross-check the reported fuel-air ratios.

### Boundary interpretation

`Intake Manifold Pressure/Temperature` and `Exhaust Manifold Pressure/Temperature` are the quantities reported by Table 2.1. They are excellent 0-D/1-D engine boundary data.

They are **not silently relabelled** as:
- compressor outlet pressure/temperature,
- compressor pressure ratio,
- turbine inlet temperature measured at the turbine flange,
- turbine pressure ratio.

The exhaust-manifold temperature is physically upstream of the downstream TEG/EGR analysis and is usable as the engine hot-side boundary, but sensor placement relative to the turbine flange should remain explicit.

### P5A validation role

**Strong:** same-point brake output + fuel + fresh air + EGR + intake manifold p/T + exhaust manifold p/T.

**Still U:** cylinder pressure, burn rate, FMEP/motoring, turbo maps/shaft speed, detailed injection schedule.

---

# P5B — Caterpillar 3401 NRC high-load build

Primary paper: Dev, Guo & Liko (2020), *A Study on the High Load Operation of a Natural Gas-Diesel Dual-Fuel Engine*, Frontiers in Mechanical Engineering 6:545416.

- HTML: https://www.frontiersin.org/journals/mechanical-engineering/articles/10.3389/fmech.2020.545416/full
- PDF: https://www.frontiersin.org/journals/mechanical-engineering/articles/10.3389/fmech.2020.545416/pdf

Only the **diesel-only αNG = 0% cases** are used below for diesel-engine validation.

## Exact HARDWARE and fuel

Source: **Table 1, article p.3** and experimental-setup text.

| Item | Value | Grade |
|---|---:|:---:|
| Engine | Caterpillar 3401 / 3400-series research engine | D |
| Cylinders | 1 | D |
| Bore × stroke | 137.2 × 165.1 mm | D |
| Displacement | 2.44 L | D |
| Compression ratio | 16.25:1 | D |
| Valves | 4: 2 intake + 2 exhaust | D |
| Stock max power | 74.6 kW at 2100 rpm | D |
| Diesel system | custom common-rail DI | D |
| Injector | Ganser six-hole solenoid | D |
| Exact hole diameter | U | U |
| Diesel density | 814.8 kg/m³ | D |
| Cetane number | 44 | D |
| Diesel LHV | 44.64 MJ/kg at 15°C, 1 atm | D |
| Diesel H/C | 1.90 | D |
| Air-flow meter | Sierra thermal-wire mass flowmeter | D |
| Diesel-flow meter | Bronkhorst | D |
| Cylinder pressure | Kistler 6041A, flush water-cooled | D |
| Crank encoder | AVL 365 series | D |
| Coolant temperature | 85°C | D |
| Oil temperature | 85°C | D |
| EGR hardware | loop present but unused in this study | D |
| EGR rate | 0% | D |

Related NRC CAT3401 publications repeatedly give **261.62 mm rod length** and valve events around IVO −358.3°, IVC −169.7°, EVO 145.3°, EVC 348.3° ATDC. Because the exact 2020 high-load paper does not print those values, retain them as **F**, not D.

## Exact diesel operating conditions

Source: **Table 2, article p.4**.

| Case | Speed rpm | BMEP bar | Brake torque Nm R | Brake power kW R | Intake T °C | Rail pressure bar | Intake p bar abs | Exhaust p bar abs | EGR | Diesel SOI |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---|
| 75% diesel baseline | 1010 | 12.86 | 249.7 | 26.41 | 40 | 525 | 1.6 | 1.3 | 0% | -18.0° ATDC |
| 100% diesel baseline | 1120 | 17.65 | 342.7 | 40.19 | 40 | 525 | 2.2 | 1.6 | 0% | varied; exact two Fig.4 diesel values not safely recovered |

Derivations:
- `T = BMEP × Vd / (4π)`
- `Pb = BMEP × Vd × rpm / 120`

At 75% load, Figure 3 explicitly fixes diesel SOI at **−18 CAD ATDC**. At 100% load, Figure 4 contains advanced/retarded diesel traces, but the accessible raster crop did not expose the legend clearly enough to assign the exact SOI values without guessing; they remain U in this extraction.

## Cylinder-pressure acquisition

Source: experimental methods, article pp.3–4.

- 100 consecutive cycles stored.
- native pressure resolution: **0.2 CAD**.
- net HRR derived from pressure.
- CA10 / CA50 / CA90 derived from heat release.

This is direct measured cylinder pressure, not MODEL-DERIVED pressure.

## Digitised diesel-only cylinder pressure

### 75% load: 12.86 bar BMEP, 1010 rpm, SOI −18 CAD ATDC

Source: **Figure 3A, article p.5**, αNG = 0% solid blue trace.

**Digitisation:** manual visual digitisation from the public raster figure, 5 CAD spacing. Approximate uncertainty **±2–3 bar and ±1 CAD**. The native experiment was 0.2 CAD; this digitised table is intentionally much coarser.

| CAD ATDC | Cylinder pressure bar |
|---:|---:|
| -30 | ~22 |
| -25 | ~27 |
| -20 | ~35 |
| -15 | ~45 |
| -10 | ~56 |
| -5 | ~65 |
| 0 | ~85 |
| 5 | ~98 |
| 10 | ~106 |
| 15 | ~101 |
| 20 | ~87 |
| 25 | ~71 |
| 30 | ~58 |

### 100% load: 17.65 bar BMEP, 1120 rpm

Source: **Figure 4A, article p.5**, αNG = 0% diesel traces.

The figure shows two diesel-only traces (advanced and retarded injection). Because the accessible figure crop did not expose the line-style/SOI legend sufficiently, the traces are labelled by relative timing rather than assigning an unverified SOI.

**Digitisation:** 5 CAD spacing; approximate uncertainty **±3 bar and ±1 CAD**.

#### Retarded diesel trace

| CAD ATDC | Cylinder pressure bar |
|---:|---:|
| -15 | ~66 |
| -10 | ~76 |
| -5 | ~89 |
| 0 | ~105 |
| 5 | ~122 |
| 10 | ~132 |
| 15 | ~130 |
| 20 | ~119 |
| 25 | ~102 |
| 30 | ~83 |
| 35 | ~68 |
| 40 | ~56 |
| 45 | ~45 |

#### Advanced diesel trace

| CAD ATDC | Cylinder pressure bar |
|---:|---:|
| -15 | ~66 |
| -10 | ~79 |
| -5 | ~95 |
| 0 | ~118 |
| 5 | ~137 |
| 10 | ~145 |
| 15 | ~139 |
| 20 | ~124 |
| 25 | ~104 |
| 30 | ~84 |
| 35 | ~68 |
| 40 | ~55 |
| 45 | ~44 |

## Heat-release-rate status

Figures 3B and 4B publicly contain **measured-pressure-derived net HRR curves** in J/CAD. The accessible web image in this pass did not provide panel B at sufficient resolution for defensible numerical digitisation. Therefore:

- existence and method: **D**
- numeric HRR arrays: **U in this Stage-2 extraction**

No values are invented.

## Motoring / friction

The paper states that a DC motor is used for starting and motoring and that each test sequence begins with steady **600 rpm motoring**. No numeric motoring torque or FMEP table was found.

- motoring procedure: **D**
- FMEP / motoring torque: **U**

---

# Combined acceptance assessment

| Validation quantity | P5A ISX | P5B CAT3401 | Combined |
|---|---|---|---|
| Large-bore geometry | D core | D | strong |
| Brake output | D | D/R | strong |
| Fuel flow | D | measured but point values not printed in chosen paper | **covered by ISX** |
| Fresh-air flow | D | measured but point values not printed | **covered by ISX** |
| BSFC | R from D data | U at selected cases | **covered by ISX** |
| Boost/intake pressure | D manifold p | D controlled boundary | strong |
| Exhaust manifold/back pressure | D | D controlled boundary | strong |
| Hot-side temperature | D exhaust manifold T | exhaust-T plots exist | strong via ISX |
| EGR rate | D | 0% in selected CAT cases | strong |
| Cylinder p(θ) | U | **D measured; digitised here** | **covered by CAT** |
| Burn/HRR | U | D existence; numeric U in this extraction | partial |
| Injection pressure/SOI | U | D at CAT | **covered by CAT** |
| Fuel density/LHV | U | D at CAT | **covered by CAT** |
| Coolant/oil T | U | D 85/85°C | **covered by CAT** |
| Turbo map/shaft speed | U | N/A | remaining gap |
| FMEP/motoring numeric | U | U | remaining gap |

## Decision

The paired replacement **meets the minimum useful validation criterion** without mixing engines:

- P5A supplies same-point **fuel + fresh air + brake output + intake/exhaust manifold p/T + EGR**.
- P5B supplies **large-bore measured cylinder pressure**, injection pressure/SOI, and tightly controlled thermal/gas-exchange boundaries.

The remaining missing capabilities are specifically **turbo maps/shaft speed** and **numeric FMEP/motoring torque**.

