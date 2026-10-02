# Stage 2 extraction prompts (2026-10-02)

One prompt per reference engine (COVERAGE_MATRIX.md §3), plus two Stage 1 searches for the new slots.

Each prompt is self-contained. The common requirements block is repeated inside each so it can be pasted alone.

Common requirements (included in every prompt):
- Return **numbers**, not descriptions. For each value give the unit, the exact source (URL + page / table / figure) and a grade:
  - **D** — exact engine and test configuration;
  - **F** — same family / closely related configuration;
  - **R** — derived by you from D/F values (show the arithmetic);
  - **U** — not found.
- Never estimate silently. Mark gaps as U.
- Where data exist only as plotted curves, digitise them: give (x, y) pairs at a stated resolution and say so.
- Downloadable files (CSV, XLSX, STL, GT-Power decks): list URL, file name and what each contains. Extract the numeric tables listed below.
- Keep one test configuration. Do not merge data from different builds, compression ratios, cams or injectors without flagging it.
- Output: one Markdown pack plus a JSON file with the same values (`{name, value, unit, grade, source}`).

---

## P1 — University of Michigan TCC-III (fundamental SI reference)

```
Extract a validation-grade data pack for the University of Michigan TCC-III optical
SI engine (571.7 cc single cylinder; Volker Sick group downloads, TCC-III CFD input
dataset, LES benchmark papers) for a 0-D / quasi-dimensional engine simulator.

Common requirements: return numbers with unit, exact source (URL + page/table/figure)
and grade (D exact configuration, F family, R derived with arithmetic shown, U not found).
Never estimate silently. Digitise plotted curves as (x, y) pairs and say so. List all
downloadable files (URL, name, contents) and extract the numeric tables requested. Keep
one test configuration; flag any mixing. Output a Markdown pack plus JSON
{name, value, unit, grade, source}.

Inputs needed:
A. bore, stroke, connecting-rod length, compression ratio / clearance volume, piston
   crown and head geometry (pent-roof, volume), wrist-pin offset.
B. valve count and head diameters, seat diameters, stem diameters; intake and exhaust
   lift vs crank angle (full arrays from the public files), timing reference (TDC
   definition), valve clearance.
C. intake: plenum volume, runner length / diameter, throttle; exhaust: runner /
   primary length / diameter, surge-tank volumes; measured valve flow coefficients
   (Cd or flow vs lift) if published.
D. fuel (composition / LHV / AFR), spark timing, lambda, swirl / tumble ratio at
   each operating condition.
E. coolant / oil / wall temperatures (head, liner, piston) if published.

Outputs needed for each published operating condition (motored and fired):
- speed, intake (MAP) and exhaust pressure, intake temperature;
- trapped / delivered air mass or air flow;
- crank-angle cylinder pressure (mean cycle; numeric, with crank-angle resolution);
- intake-port and exhaust-port pressure traces if published;
- IMEP, COV of IMEP, burn angles (CA10 / 50 / 90) if published;
- any brake data (unlikely; mark U).
```

---

## P2 — Honda GX160 (small SI with brake performance)

```
Extract a validation-grade data pack for the Honda GX160 (163 cc, 68 x 45 mm,
single-cylinder OHV, carburetted, air-cooled) for a 0-D engine simulator, focusing on
studies that combine dynamometer data with cylinder pressure (e.g. 1-D simulation /
pressure-model studies, manifold optimisation / dyno studies, university theses).

Common requirements: return numbers with unit, exact source (URL + page/table/figure)
and grade (D exact configuration, F family, R derived with arithmetic shown, U not found).
Never estimate silently. Digitise plotted curves as (x, y) pairs and say so. List all
downloadable files and extract the numeric tables requested. Keep ONE engine build
and test set-up per study; never merge studies without flagging it. Output a Markdown
pack plus JSON {name, value, unit, grade, source}.

Inputs needed:
A. geometry: rod length, compression ratio (measured if available), piston pin offset.
B. valve head / seat / stem diameters; lift vs crank angle or max lift + opening /
   closing angles with the lift threshold used; valve clearance.
C. carburettor venturi and throttle diameters; intake path length (carb to valve);
   air cleaner (type, restriction); exhaust port / pipe / muffler geometry and
   back-pressure; any measured flow coefficients.
D. fuel, spark timing (fixed advance), lambda / AFR vs speed at full load if measured.
E. cylinder-head / oil temperature if measured.

Outputs needed (per study, same configuration):
- full-load torque and power vs speed (numeric);
- BSFC (or fuel flow) vs speed at full load, and at part load if available;
- AIR FLOW vs speed (critical: separates breathing from efficiency);
- cylinder pressure traces at stated speed / load (numeric);
- exhaust temperature, intake depression;
- motoring / friction data if any.
State which of these come from the same physical test set-up.
```

---

## P3 — Kirloskar TV1 and AVL 5402 (small diesel)

```
Extract validation-grade data packs for two small single-cylinder DI diesel research
engines for a 0-D engine simulator: (1) Kirloskar TV1 (661 cc, 87.5 x 110 mm, CR 17.5,
MICO inline pump, mechanical governor) and (2) AVL 5402 (511 cc, common rail). Treat
them as two separate packs.

Common requirements: return numbers with unit, exact source (URL + page/table/figure)
and grade (D exact configuration, F family, R derived with arithmetic shown, U not found).
Never estimate silently. Digitise plotted curves as (x, y) pairs and say so. List all
downloadable files and extract the numeric tables requested. Keep one engine build and
test set-up per study; flag any mixing (many labs modify these engines). Output a
Markdown pack plus JSON {name, value, unit, grade, source}.

Inputs needed (each engine):
A. bore, stroke, rod length, CR, bowl geometry (diameter, depth, volume).
B. valve diameters, lift vs angle or max lift + timing, valve clearance.
C. intake and exhaust pipe geometry as installed in the test cell (lengths, diameters,
   surge tanks), air-flow measurement method.
D. injection: pump type, plunger diameter and cam lift rate (TV1), rail pressure (AVL),
   nozzle holes x diameter, spray angle, opening pressure, static and dynamic injection
   timing, injection rate or duration if measured, fuel properties (LHV, cetane).
E. coolant / oil temperatures.

Outputs needed (each operating point, same set-up):
- speed, load (torque or BMEP), fuel flow, AIR FLOW, BSFC;
- crank-angle cylinder pressure and heat-release rate (numeric);
- ignition delay, combustion duration;
- exhaust temperature;
- motoring pressure / friction if available.
```

---

## P4 — GM / Opel 1.9 L ECN small-bore diesel (diesel physics reference)

```
Extract a validation-grade data pack for the Sandia Engine Combustion Network (ECN)
small-bore diesel engine (GM / Opel 1.9 L derived, single-cylinder optical / metal
versions) from the ECN repository (ecn.sandia.gov/engines/small-bore-diesel-engine/)
for a 0-D engine simulator.

Common requirements: return numbers with unit, exact source (URL + page/table/figure)
and grade (D exact configuration, F family, R derived with arithmetic shown, U not found).
Never estimate silently. Digitise plotted curves as (x, y) pairs and say so. List all
downloadable files (URL, name, contents) and extract the numeric tables requested. State
clearly which hardware version (optical or all-metal, piston bowl, CR) each data set
belongs to. Output a Markdown pack plus JSON {name, value, unit, grade, source}.

Inputs needed:
A. bore, stroke, rod length, CR, piston bowl geometry and volume, squish height.
B. valve diameters, intake and exhaust lift vs crank angle (arrays), measured valve
   flow coefficients and swirl ratio (vs lift).
C. intake / exhaust plumbing as run (surge tanks, pressures), boost conditions.
D. injector: holes x diameter, included angle, rail pressure, injection rate shape
   (numeric), SOI / duration commands and hydraulic delays, fuel (composition, LHV,
   cetane).
E. wall / coolant / oil temperatures.

Outputs needed for each published operating point:
- speed, intake pressure / temperature, O2 concentration (EGR), fuel mass per cycle;
- crank-angle cylinder pressure and apparent heat-release rate (numeric);
- IMEP; motored pressure trace;
- emissions / soot only if alongside the above.
```

---

## P5 — Volvo D13 (heavy-duty diesel)

```
Extract a validation-grade data pack for the Volvo D13 heavy-duty diesel (12.8 L
I6, 131 x 158 mm) as used in the Chalmers GT-Power validation thesis (12 ESC points)
and any related Chalmers / Volvo publications, for a 0-D engine simulator.

Common requirements: return numbers with unit, exact source (URL + page/table/figure)
and grade (D exact configuration, F family, R derived with arithmetic shown, U not found).
Never estimate silently. Digitise plotted curves as (x, y) pairs and say so. List all
downloadable files and extract the numeric tables requested. Keep one engine
build / rating; flag any mixing. Output a Markdown pack plus JSON
{name, value, unit, grade, source}.

Inputs needed:
A. rod length, CR, firing order, bowl geometry if published.
B. valve diameters, lift profiles or timing; any engine-brake / VVA hardware.
C. intake / exhaust manifold and pipe geometry; charge-air cooler; EGR route; turbo
   (VGT type, maps or measured operating points: pressure ratios, turbine inlet
   temperature).
D. injection: system, holes x diameter, rail pressure, SOI / quantity per point;
   fuel properties.
E. coolant / oil temperatures.

Outputs needed for each ESC point (and any full-load curve):
- speed, torque / BMEP, fuel flow, air flow, BSFC;
- boost, exhaust manifold pressure, turbine inlet temperature, EGR rate;
- cylinder pressure and burn rate (numeric);
- runner pressure traces if published;
- motoring / FMEP data if available.
Also give the documented full-load torque / power curve of the same rating.
```

---

## S8 — Stage 1 search: handheld two-stroke < 100 cc

```
Stage 1 search (bounded): find two-stroke spark-ignition engines below 100 cc
(chainsaw, trimmer, leaf blower, model / small motorcycle) for which public sources give
both simulator inputs and measured outputs, for validating a two-stroke module
(crankcase scavenging, port timing, tuned exhaust) in a 0-D / 1-D engine simulator.

For each candidate (aim for 2-3, do not pad), score availability as D (exact
configuration) / P (partial or related) / U (not found) for:
A geometry (bore, stroke, rod, crankcase compression ratio, trapped CR);
B port timing and areas (exhaust, transfer, intake / reed or piston-port), port shapes;
C intake (carb, reed) and exhaust (expansion chamber / muffler) geometry;
D fuel, mixture, spark timing;
E thermal;
G full-load torque / power curve;
H fuel flow and AIR FLOW / delivery ratio / trapping efficiency;
I cylinder and crankcase pressure traces;
J scavenging measurements (tracer gas, short-circuit losses), exhaust pressure traces.
Give source URLs (papers, theses, open datasets), data form (tables / plots / files),
and the main gaps. Conclude with a recommended candidate and a Stage-2 question.
```

---

## S9 — Stage 1 search: locomotive / marine medium-speed diesel

```
Stage 1 search (bounded): find medium-speed diesel engines (bore ~200-260 mm,
~900-1100 rpm; locomotive, marine or stationary; four-stroke e.g. ALCO 251, GE FDL,
MAN, Wartsila; and two-stroke e.g. EMD 567 / 645 / 710) for which public sources give
both simulator inputs and measured outputs, for validating heat transfer, friction and
combustion at large bore in a 0-D engine simulator.

For each candidate (aim for 2-3, do not pad), score availability as D / P / U for:
A geometry (bore, stroke, rod, CR, cylinder count, firing order);
B valves / ports and timing;
C intake / exhaust and turbocharger / Roots blower arrangement;
D injection system (pump / unit injector), timing, nozzle data, fuel;
E thermal;
F turbo / blower data (maps or measured boost, turbine inlet temperature);
G load curve (power vs speed / notch);
H SFC and air flow at the same points;
I cylinder pressure (peak pressure at least);
J exhaust temperature, friction / motoring, auxiliary loads.
Prefer research papers, theses, service and maintenance manuals (e.g. MI-series),
classification-society or emissions certification data. Give URLs, data form and gaps.
Conclude with a recommended candidate per cycle type (four-stroke, two-stroke).
```
