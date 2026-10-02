# P1 — University of Michigan TCC-III validation-grade data pack

**Status:** current working pack, 2026-10-02.  
**Pending update:** the **TCC-III Motored Full View archive is currently being transferred and has not yet been ingested**. Exact primary valve-lift files, GT-Power `.gtm/.gdx`, CAD `.igs/.stl`, pressure workbooks and the complete internal file manifest will be added when that archive is available. Fields that require those raw files remain **U** in this version even where the README proves that exact data exist.

The Fired Full View and Fired Spark Plug Region READMEs have been incorporated. Their very large raw ZIP archives are not currently ingested; raw numeric arrays which exist only inside those archives are likewise marked U/pending rather than silently reconstructed.

## Conventions

Importance: **[E] essential**, **[I] important**, **[N] nice to have**.  
Grades: **D** exact TCC-III configuration/source; **F** family/secondary implementation; **R** derived, arithmetic stated; **U** not presently found/extracted.  
Crank angle: **ATDCE = degrees after TDC exhaust**; exhaust TDC = 0/720 CAD and compression TDC = 360 CAD.

### Configuration lock

The pack is locked to the University of Michigan **TCC-III Full View** hardware. TCC-II dimensions are not used to fill missing TCC-III values. The Spark Plug Region is retained as a **separate TCC-III fired campaign** and is not used to replace missing Full View operating values. The public OpenFOAM `tcc3` case is kept only under **MODEL-DERIVED / SECONDARY**.

TCC-III is **not pent-roof**: the optical engine uses the flat/pancake TCC chamber arrangement.

Primary sources:

- Motored Full View: https://doi.org/10.7302/Z2MS3QP
- Fired Full View: https://doi.org/10.7302/Z2H41PCD
- Fired Spark Plug Region: https://doi.org/10.7302/Z2CC0XNP
- LES benchmark: https://doi.org/10.2516/ogst/2015028
- Secondary OpenFOAM case: https://github.com/OpenFOAM/ICengines/tree/master/tutorials/tcc3

---

# 1. INPUTS — HARDWARE / MEASURED

## A. Geometry

| Need | Quantity | Value | Grade | Source |
|---|---|---:|:---:|---|
| [E] | Bore | 92.0 mm | D | Motored README p.11 |
| [E] | Stroke | 86.0 mm | D | Motored README p.11 |
| [E] | Connecting rod | 231.0 mm | D | Motored README p.11 |
| [E] | Geometric compression ratio | 10.0:1 | D | Motored README p.11 |
| [E] | Effective IVC compression ratio | 8.0:1 | D | Motored README p.11 |
| [E] | TDC volume | 63.54 cm³ | D | Motored README p.11 |
| [E] | TDC clearance | 9.5 mm | D | Motored README p.14 |
| [I] | Combustion-chamber volume | 63.15 cm³ | D | Motored README p.11 |
| [I] | Top-land crevice | 0.37 cm³ | D | Motored README p.11 |
| [I] | Spark-plug crevice | 0.02 cm³ | D | Motored README p.11 |
| [I] | Chamber/head/piston form | pancake; flat TCC optical arrangement | D | Motored README pp.12–16 |
| [I] | Piston OD | 91.35 mm | D | Motored README p.14 |
| [I] | Top-land height | 4.0 mm | D | Motored README p.14 |
| [I] | Quartz piston window | 69.8 mm dia | D | Motored README p.14 |
| [I] | Head/piston surface areas | U | U | not tabulated |
| [N] | Piston-pin offset | 0.0 mm | D | Motored README p.11 |

## B. Valves

**Valve count:** 2 — **D**.

Exact timing, all in CAD ATDCE: EVC **12.8**, intake peak-lift timing **114.8**, IVC **240.8**, EVO **484.8**, exhaust peak-lift timing **606.8**, IVO **712.8** — **D**, Motored README p.11.

### TCC-III four-angle seat

Motored README p.21 identifies the **green profile** as TCC-III:

| Feature | Value | Grade |
|---|---:|:---:|
| Port ID on seat drawing | 25.4 mm | D |
| 75° cut | 2.1 mm | D |
| 60° cut | 1.6 mm | D |
| 45° cut | 1.3 mm | D |
| 30° cut | 2.0 mm | D |
| 75° reference OD | 26.44 mm | D |
| 60° reference OD | 28.04 mm | D |
| 45° reference OD | 29.9 mm | D |
| Seat insert OD | 33.4 mm | D |

**[E] Valve head diameters:** U.  
**[I] Stem diameter:** U.  
**[I] Valve clearance/lash:** U.

### [E] Full lift arrays

The Motored README directory screenshot explicitly contains a `TCCIII_Valve_Lifts` directory, but the raw archive has not yet been ingested. Therefore:

- primary TCC-III intake lift vs CAD: **U — PENDING MOTored ARCHIVE**
- primary TCC-III exhaust lift vs CAD: **U — PENDING MOTored ARCHIVE**

An interim secondary OpenFOAM implementation exists at:

- `https://raw.githubusercontent.com/OpenFOAM/ICengines/master/tutorials/tcc3/constant/engineData/intakeLift.txt`
- `https://raw.githubusercontent.com/OpenFOAM/ICengines/master/tutorials/tcc3/constant/engineData/exhaustLift.txt`

These are **F**, not substitutes for the pending primary UM arrays. They are already numeric files, so no curve digitisation is involved.

## C. Intake/exhaust system

### Plenums and pressure locations

Motored README p.13 gives the main interior dimensions:

- intake plenum main section: 609.6 mm high × 101.6 mm depth × (88.9 + 88.9) = **177.8 mm width** — D/D/R
- exhaust plenum main section: 609.6 mm high × 101.6 mm depth × (44.5 + 133.4) = **177.9 mm width** — D/D/R
- flame arrestor: **23 mm ID × 150 mm long** — D
- intake-plenum inlet and exhaust-plenum outlet P/T measurement positions are explicitly shown.

**[E] Exact total plenum volumes: U.** The lower plenum-to-runner transition is not a rectangular prism, so using the rectangular dimensions alone would overstate certainty. Exact volume is expected to be recoverable from the pending `.gtm/.gdx` or CAD files.

### Runner/port interfaces

Motored README p.19:

- runner/port probe-insert hole: **25.4 mm**, matching runner/port diameter — D
- plenum outlet-pipe ID at plenum probe insert: **19.1 mm** — D
- p.19 says probe insert thickness **25.4 mm**
- pp.17–18 instead label **25.0 mm insert + 2 × 1.6 mm gaskets**.

That 25.0/25.4 mm discrepancy is retained explicitly; it is **not silently reconciled**.

### Intake runner drawing — Motored README p.17

Exact labelled drawing quantities include: bell-mouth radius **38.10 mm**, bend radius **76.20 mm**, lower bend radius **60.0 mm**, labelled radius **136.20 mm**, and dimensions **17.46, 4.76, 106.5, 13.3, 28.9, 28.2, 60.0, 25.4, 6.38, 3.25 mm**, with a **45°** port-axis annotation.

### Exhaust runner drawing — Motored README p.18

Exact labelled quantities include radii **76.2, 60.0, 136.20 mm** and dimensions **27.0, 68.2, 28.9, 13.3, 28.2, 28.58, 19.1, 60.0, 25.4, 6.38 mm**.

These drawings are already enough to constrain the physical layout, but **[E] the fully ordered 1-D segment table is still U/pending** because interpreting every centerline/transition from a scanned drawing would require assumptions. The pending GT-Power/CAD files are the authoritative route.

### Upstream intake supply — Motored README p.31

The archive documents, among other segments:

- metering outlet → heater: **1.91 cm ID × 288 cm**
- air heater: **5.1 cm ID × 34 cm**
- heated line → flame arrestor: **5.1 cm ID × 150 cm**
- flame arrestor: **2.3 cm ID × 15 cm**
- PIV atomizer: **14 × 14 × 7.5 cm**
- PIV-atomizer branch: **1.91 cm ID × 265 cm**
- additional branch: **1.91 cm ID × 300 cm**
- fuel branch to mixer: **0.95 cm ID × 305 cm**

The critical-flow air, nitrogen and fuel metering branches are also dimensioned on p.31. They are upstream supply hardware; for the simulator the natural measured boundary may be the intake plenum.

### Downstream exhaust supply/boundary — Motored README p.32

Documented hardware includes a **5.1 cm ID, 355 cm** common header split into labelled **163 + 40 + 135 cm** sections; exhaust-plenum branch pieces of **1.9 cm ID × 15 cm**, **1.9 cm ID × 60 cm**, and **5.1 cm ID × 120 cm**; and a laboratory exhaust of **102 cm ID** with unknown length and boundary `P = P_ambient - 1.5 kPa`.

The throttling valve is adjusted to achieve **P_exh MAP = 101.5 kPa**.

**[I] throttle geometry/position:** U.  
**[I] measured valve flow/Cd vs lift:** U, pending search inside the GT-Power/geometry archive.

## D. Fuel / spark / lambda / charge motion

Canonical Fired Full View:

- speed **1300 rpm**, MAP **40 kPa**, φ **1** — D
- SOIgn **342 CAD ATDCE**, explicitly stated as MBT timing — D, Fired README p.2
- λ = `1/φ = 1.0` — R
- catalog metadata identifies the canonical Full View fuel as premixed stoichiometric propane/air — D
- fuel LHV — **U**
- source-specific stoichiometric AFR — **U**
- ideal dry-air propane AFR, for reference only: `C3H8 + 5(O2+3.76N2)` → **15.57143 kg_air/kg_fuel — R**, not a replacement for the source-specific value.
- steady-flow swirl ratio **0.4** — D
- tumble ratio — **U**

## E. Thermal

Canonical motored benchmark:

- intake-air set point **45 °C** — D
- engine-oil inlet **45 °C** — D
- coolant outlet **45 °C** — D
- outside quartz-cylinder wall: **36.9 / 40.8 / 41.0 °C** at 800/95, 1300/95 and 1300/40 respectively — D.

Exact separate head/liner/piston wall temperatures: **U**.

---

# 2. OUTPUTS

## Motored Full View — canonical three operating conditions

Source for scalar measurements: Schiffmann et al. LES benchmark, Appendix A Table A2.  
For future raw trace extraction, this pack selects a **y=0 archive test at each condition**, avoiding a change in measurement-plane convention. The pressure channels themselves are engine sensors rather than PIV-plane quantities.

| rpm | MAP set kPaa | exhaust set kPaa | intake T °C | mean intake-port kPaa | mean exhaust-port kPaa | delivered air g/s | IMEP kPa | peak cyl kPaa @ CAD | wall °C | selected raw test |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---|
| 800 | 95 | 101.5 | 45 | 94.6 | 101.7 | 3.50 | -41.9 | 1810 @ 358.6 | 36.9 | `S_2014_05_20_02` |
| 1300 | 95 | 101.5 | 45 | 94.4 | 101.8 | 5.88 | -37.8 | 1958 @ 358.8 | 40.8 | `S_2014_05_20_01` |
| 1300 | 40 | 101.5 | 45 | 39.5 | 101.1 | 2.07 | -19.6 | 798 @ 358.5 | 41.0 | `S_2013_10_24_01` |

### [E] Motored numeric crank-angle traces

The Motored README pp.3–6 confirms that **each test has one Excel pressure workbook** containing:

- `Ensemble_Average`
- `P_IntakePlenIn`
- `P_IntkPort`
- `P_Cyl`
- `P_ExhPort`
- `P_ExhPlenOut`

All five pressures are acquired every **0.5 CAD**.

**Current pack status:** the primary arrays are **U/pending archive ingestion**. They are known to exist, but this pack does not pretend to contain numbers that have not yet been extracted.

**[I] motored COV IMEP:** U; the benchmark gives IMEP scatter/standard deviation, not an explicitly labelled COV.  
**[I] CA10/50/90:** not applicable motored.  
**[N] brake data:** U.

## Fired Full View — 1300 rpm / 40 kPa / φ=1

All tests in the README p.2 use **1300 rpm, 40 kPa MAP, φ=1**, SOIgn **342 CAD ATDCE**.

| PIV plane / subset | Test ID | dissertation-recommended | IMEP kPa | COV IMEP % |
|---|---|:---:|---:|---:|
| x=0 | `S_2014_02_05_01` | no | 333 | 0.6 |
| x=0 | `S_2014_02_12_01` | no | 333 | 0.7 |
| x=0 | `S_2014_02_13_02` | yes | 323 | 0.5 |
| y=0 | `S_2013_10_29_01` | yes | 343 | 1.9 |
| y=0 | `S_2013_10_31_02` | no | 333 | 0.8 |
| y=0 | `S_2013_11_07_03` | no | 333 | 0.7 |
| z=-5 | `S_2014_05_06_01` | no | 343 | 1.9 |
| z=-5 | `S_2014_05_08_01` | yes | 329 | 0.9 |
| z=-5 | `S_2014_05_13_01` | no | 326 | 1.2 |
| z=-30 | `S_2014_04_01_01` | no | 332 | 1.1 |
| z=-30 | `S_2014_04_03_02` | yes | 329 | 0.7 |
| Fired SS | `S_2014_04_16_03_B` | no | 334 | 0.6 |

To prevent test mixing, the pack designates **`S_2014_02_13_02` (x=0)** as the canonical fired raw-data test because it is explicitly blue-bold/recommended in the archive README. The other tests remain listed as repeats/alternative PIV-plane datasets, not averaged into it.

For every fired test, the README confirms the same five **0.5-CAD** pressure channels as motored, with additional per-cycle heat-release and spark-plasma information.

Current status for the selected fired test:

- [E] speed: **1300 rpm D**
- [E] MAP: **40 kPaa D**
- [E] exhaust-plenum pressure set point: **101.5 kPaa D**
- [E] intake temperature: **U**
- [E] delivered air flow: **U**
- [E] mean-cycle cylinder pressure array: **U — exact workbook exists but raw archive not ingested**
- [E] intake-port pressure array: **U — exact workbook exists**
- [E] exhaust-port pressure array: **U — exact workbook exists**
- [I] IMEP: **323 kPa D**
- [I] COV IMEP: **0.5% D**
- [I] CA10 / CA50 / CA90: **U in current Full View extraction**; heat-release data exist and can support derivation once raw workbook is supplied.
- [N] brake data: **U**

## Separate fired campaign — Spark Plug Region

**Do not merge its mixture values into the canonical Full View test.** It is retained because it provides unusually rich combustion-validation coverage.

All conditions: **1300 rpm, 40 kPa MAP, SOIgn 342 CAD ATDCE**. The README p.2 gives the following operating-condition summary:

| # | fuel | dilution | φ | λ | O₂ g/s | N₂ g/s | primary fuel g/s | C7H8 g/s | reported A/F | IMEP kPa | COV % |
|---:|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 | CH4 | air | 0.66 | 1.515 R | 0.48 | 1.59 | 0.070 | 0.012 | 25.3 | 212 | 8.3 |
| 2 | CH4 | air | 0.69 | 1.449 R | 0.48 | 1.59 | 0.075 | 0.012 | 24.0 | 250 | 4.5 |
| 3 | CH4 | air | 1.00 | 1.000 R | 0.47 | 1.56 | 0.109 | 0.012 | 16.8 | 328 | 1.3 |
| 4 | CH4 | air | 1.21 | 0.826 R | 0.47 | 1.54 | 0.132 | 0.012 | 14.0 | 287 | 4.5 |
| 5 | C3H8 | air | 0.67 | 1.493 R | 0.48 | 1.58 | 0.078 | 0.012 | 23.0 | 234 | 6.3 |
| 6 | C3H8 | air | 1.00 | 1.000 R | 0.47 | 1.55 | 0.119 | 0.012 | 15.5 | 337 | 0.8 |
| 7 | C3H8 | air | 1.56 | 0.641 R | 0.46 | 1.50 | 0.185 | 0.012 | 10.0 | 259 | 6.3 |
| 8 | C3H8 | 9% N2 by mass | 0.79 | 1.266 R | 0.44 | 1.62 | 0.084 | 0.012 | 19.6 | 250 | 5.0 |
| 9 | C3H8 | 9% N2 by mass | 1.00 | 1.000 R | 0.43 | 1.60 | 0.108 | 0.012 | 15.5 | 311 | 1.8 |
| 10 | C3H8 | 9% N2 by mass | 1.43 | 0.699 R | 0.42 | 1.57 | 0.154 | 0.012 | 10.8 | 278 | 5.5 |
| 11 | C3H8 | 19% N2 | 1.00 | 1.000 R | 0.38 | 1.66 | 0.096 | 0.012 | 15.5 | 212 | 8.2 |

Spark Plug Region README p.4 states that its consolidated workbook contains **0.5-CAD ensemble pressure and apparent heat-release data for all 34 tests**. README p.8 explicitly lists **CA10, CA50, CA90, Burn0010, Burn1090, total heat release, spark duration and spark energy** as numeric per-cycle fields.

The raw 134 GB archive is not ingested, so the actual CA10/50/90 and pressure arrays remain **U/pending**, not estimated.

---

# 3. MODEL-DERIVED / SECONDARY — DO NOT MIX WITH HARDWARE

A public OpenFOAM `tutorials/tcc3` implementation provides numeric secondary files:

- intake lift: `https://raw.githubusercontent.com/OpenFOAM/ICengines/master/tutorials/tcc3/constant/engineData/intakeLift.txt`
- exhaust lift: `https://raw.githubusercontent.com/OpenFOAM/ICengines/master/tutorials/tcc3/constant/engineData/exhaustLift.txt`
- chamber pressure: `https://raw.githubusercontent.com/OpenFOAM/ICengines/master/tutorials/tcc3/constant/engineData/chamberPressure.txt`
- intake pressure: `https://raw.githubusercontent.com/OpenFOAM/ICengines/master/tutorials/tcc3/constant/engineData/intakePressure.txt`
- exhaust pressure: `https://raw.githubusercontent.com/OpenFOAM/ICengines/master/tutorials/tcc3/constant/engineData/exhaustPressure.txt`

These are **F**. They are already tabulated, not plot digitisation.

Known trace metadata from those files:

- chamber pressure: 1440 points, 0–719.5 CAD, Δ=0.5 CAD; maximum 1826.65 kPa at 358.5 CAD
- intake pressure: 1440 points, -360–359.5 CAD, Δ=0.5 CAD; mean ≈94.2607 kPa
- exhaust pressure: 1440 points, -360–359.5 CAD, Δ=0.5 CAD; mean ≈101.7225 kPa

The operating point is not explicitly identified in the files; the apparent agreement with the 800 rpm/95 kPa benchmark is **not** enough to promote them to D.

No curves have been manually digitised in this pack. Wherever a numeric source file exists, the source file is preferred.

---

# 4. DOWNLOAD / ARCHIVE INVENTORY STATUS

## Motored Full View — PENDING TRANSFER

README pp.3–7 identify at least these internal groups:

- `pressure_data`
- `common_grid_flow_fields`
- `original_grid_flow_fields`
- `TCCIII_CFD_Geometry`
- `TCCIII_Valve_Lifts`
- GT-Power 1-D `.gtm` and `.gdx`
- CFD `.igs` and `.stl`
- per-test Excel pressure workbooks
- PIV velocity text files.

**Complete file-by-file manifest:** U until the ~30 GB archive finishes transferring.  
This is the major pending update to this pack.

When available, priority extraction is:

1. `TCCIII_Valve_Lifts`
2. `.gtm` / `.gdx`
3. `.igs` / `.stl`
4. pressure workbooks for selected tests `S_2014_05_20_02`, `S_2014_05_20_01`, `S_2013_10_24_01`
5. any valve-flow/Cd tables or object parameters
6. exact archive manifest.

The multi-GB PIV directories are not required for the present 0-D/1-D pack.

## Fired Full View

The README confirms one Excel pressure workbook per test with `Test Info`, `Per_Run_Summary`, `Per_Cycle_Data`, `Ensemble_Average` and five 0.5-CAD pressure sheets. Raw archive not ingested.

## Fired Spark Plug Region

The README confirms:

- per-test pressure workbooks
- one consolidated 1/test workbook
- one 0.5-CAD ensemble pressure + heat-release workbook for all 34 tests
- one 1/cycle statistical-parameter workbook
- PIV and OH* image data.

The 134 GB image/PIV bulk is not necessary for the present 0-D/1-D reference role.

---

# 5. CURRENT ESSENTIAL GAPS

The pack cannot yet be called fully reference-complete because these **[E]** fields remain U:

1. TCC-III intake and exhaust **valve head diameters**.
2. Primary **full valve-lift arrays** — expected in the pending Motored Full View archive.
3. Fully unambiguous ordered **1-D runner/transition/plenum segment model** and exact plenum volumes — expected from `.gtm/.gdx`/CAD.
4. Source-specific **fuel LHV and stoichiometric AFR**.
5. Primary **0.5-CAD motored pressure arrays** for the three canonical operating conditions — confirmed present, pending extraction.
6. Fired Full View **intake temperature and delivered air flow**.
7. Fired Full View primary **0.5-CAD cylinder/intake/exhaust pressure arrays** — confirmed present, raw archive not ingested.

The pending Motored Full View archive is expected to close items 2, 3 and 5, and may also close valve-diameter/Cd gaps if those are embedded in the GT-Power/CAD parameter sets.

---

# 6. Publication/source note

The University of Michigan README requests acknowledgement of the publicly available TCC engine data and its General Motors / University of Michigan research funding when the data or simulation geometry are used in publication. Consult the source README p.1 for the exact requested wording.

