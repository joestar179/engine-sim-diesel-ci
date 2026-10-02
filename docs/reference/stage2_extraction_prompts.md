# Stage 2 extraction prompts (updated 2026-10-02 for the agreed plan)

One prompt per reference engine (COVERAGE_MATRIX.md §3), plus three Stage 1 searches:
- S8 handheld two-stroke;
- S9 locomotive / marine;
- S10 supercharged petrol.

Each prompt is self-contained (the common block is repeated) so it can be pasted alone.

Plan-driven additions (COVERAGE_MATRIX §5):
- **hardware-only** inputs; model parameters reported separately;
- full pipe geometry for **1-D pipes** and runner pressure traces;
- data for the **two-stroke**, **direct-injection**, **boost-control**, **carburettor-metering** and **supercharger** modules where the engine has them.

Suggested order: P2, P1 (open physics questions), then P4, P3, P5; S8 / S9 / S10 any time.

---

## P1 — University of Michigan TCC-III (fundamental SI reference)

```
Extract a validation-grade data pack for the University of Michigan TCC-III optical
SI engine (571.7 cc single cylinder; Volker Sick group downloads, TCC-III CFD input
dataset, LES benchmark papers) for a 0-D / 1-D engine simulator.

Common requirements:
- Return numbers with unit, exact source (URL + page/table/figure) and grade:
  D exact configuration, F family, R derived (show arithmetic), U not found.
  Never estimate silently.
- Report HARDWARE (physical dimensions, measured quantities). Values that exist only
  as simulation-model parameters (e.g. GT-Power discharge coefficients, friction
  multipliers, calibrated burn rates) go in a separate section labelled MODEL-DERIVED.
- Digitise plotted curves as (x, y) pairs at a stated resolution and say so.
- List every downloadable file (URL, name, contents) and extract the numeric tables
  requested.
- Keep one test configuration; flag any mixing of builds.
- Output a Markdown pack plus JSON {name, value, unit, grade, source}.

Inputs needed:
A. bore, stroke, connecting-rod length, compression ratio / clearance volume, piston
   crown and head geometry (pent-roof, volume), wrist-pin offset.
B. valve count, head / seat / stem diameters; intake and exhaust lift vs crank angle
   (full arrays from the public files); TDC reference; valve clearance.
C. Full intake and exhaust pipe geometry as run, suitable for a 1-D pipe model:
   every pipe segment from the inlet / surge tank to the valve and from the valve to
   the exhaust tank, with length, diameter (or area) along the length, tapers, bends,
   junctions, plenum / surge-tank volumes and the boundary pressures; throttle
   geometry; measured valve flow (flow or Cd vs lift) if published.
D. fuel (composition / LHV / AFR), spark timing, lambda, swirl / tumble ratio at
   each operating condition.
E. coolant / oil / wall temperatures (head, liner, piston) if published.

Outputs needed for each published operating condition (motored and fired):
- speed, intake (MAP) and exhaust pressure, intake temperature;
- trapped / delivered air mass or air flow;
- crank-angle cylinder pressure (mean cycle; numeric, crank-angle resolution);
- crank-angle intake-port and exhaust-port / runner pressure traces (needed to
  validate 1-D pipe dynamics);
- IMEP, COV of IMEP, burn angles (CA10 / 50 / 90) if published;
- any brake data (unlikely; mark U).
```

---

## P2 — Honda GX160 (small SI with brake performance)

```
Extract a validation-grade data pack for the Honda GX160 (163 cc, 68 x 45 mm,
single-cylinder OHV, float carburettor, air-cooled) for a 0-D / 1-D engine
simulator, focusing on studies that combine dynamometer data with cylinder pressure
(1-D simulation / pressure-model studies, manifold optimisation / dyno studies,
university theses).

Common requirements:
- Return numbers with unit, exact source (URL + page/table/figure) and grade:
  D exact configuration, F family, R derived (show arithmetic), U not found.
  Never estimate silently.
- Report HARDWARE (physical dimensions, measured quantities). Values that exist only
  as simulation-model parameters (e.g. GT-Power discharge coefficients, friction
  multipliers, calibrated burn rates) go in a separate section labelled MODEL-DERIVED.
- Digitise plotted curves as (x, y) pairs at a stated resolution and say so.
- List every downloadable file and extract the numeric tables requested.
- Keep ONE engine build and test set-up per study; never merge studies without
  flagging it.
- Output a Markdown pack plus JSON {name, value, unit, grade, source}.

Inputs needed:
A. geometry: rod length, compression ratio (measured if available), pin offset.
B. valve head / seat / stem diameters; lift vs crank angle, or max lift + opening /
   closing angles with the lift threshold used; valve clearance.
C. Intake path, suitable for a 1-D pipe model: air cleaner (type, volume, measured
   restriction), carburettor bore and venturi diameter, throttle, insulator / spacer
   and port, each with length and diameter. Exhaust: port, pipe and muffler with
   internal geometry (chamber volumes, baffle / perforate dimensions, tailpipe) and
   measured back-pressure. Any measured flow coefficients.
D. Carburettor metering: main jet size, air-bleed / emulsion details, float level,
   choke; measured lambda / AFR vs speed at full load and at part load. Spark timing
   (fixed advance), fuel properties.
E. cylinder-head / oil temperature if measured.

Outputs needed (per study, same configuration):
- full-load torque and power vs speed (numeric);
- BSFC (or fuel flow) vs speed at full load, and at part load if available;
- AIR FLOW vs speed (critical: separates breathing from efficiency);
- cylinder pressure traces at stated speed / load (numeric), plus burn angles if
  given;
- intake and exhaust pressure traces if measured;
- exhaust temperature, intake depression;
- motoring / friction data if any.
State which of these come from the same physical test set-up.
```

---

## P3 — Kirloskar TV1 and AVL 5402 (small diesel)

```
Extract validation-grade data packs for two small single-cylinder DI diesel research
engines for a 0-D / 1-D engine simulator: (1) Kirloskar TV1 (661 cc, 87.5 x 110 mm,
CR 17.5, MICO inline pump, mechanical governor) and (2) AVL 5402 (511 cc, common
rail). Treat them as two separate packs.

Common requirements:
- Return numbers with unit, exact source (URL + page/table/figure) and grade:
  D exact configuration, F family, R derived (show arithmetic), U not found.
  Never estimate silently.
- Report HARDWARE (physical dimensions, measured quantities). Values that exist only
  as simulation-model parameters go in a separate section labelled MODEL-DERIVED.
- Digitise plotted curves as (x, y) pairs at a stated resolution and say so.
- List every downloadable file and extract the numeric tables requested.
- Keep one engine build and test set-up per study; flag any mixing (many labs modify
  these engines).
- Output a Markdown pack plus JSON {name, value, unit, grade, source}.

Inputs needed (each engine):
A. bore, stroke, rod length, CR, bowl geometry (diameter, depth, volume).
B. valve diameters, lift vs angle or max lift + timing, valve clearance.
C. Intake and exhaust pipe geometry as installed in the test cell, suitable for a 1-D
   pipe model (segment lengths and diameters, surge tanks / air box volumes), air-flow
   measurement method.
D. Injection hardware: pump type, plunger diameter and cam lift rate (TV1), rail
   pressure (AVL), nozzle holes x diameter, spray angle, opening pressure, static and
   dynamic injection timing, injection rate shape or duration if measured; spray
   penetration / liquid length if published; fuel properties (LHV, cetane, density).
E. coolant / oil temperatures.

Outputs needed (each operating point, same set-up):
- speed, load (torque or BMEP), fuel flow, AIR FLOW, BSFC;
- crank-angle cylinder pressure and heat-release rate (numeric);
- ignition delay, combustion duration;
- exhaust temperature;
- governor characteristic (TV1: speed vs load / droop) if published;
- motoring pressure / friction if available.
```

---

## P4 — GM / Opel 1.9 L ECN small-bore diesel (diesel physics reference)

```
Extract a validation-grade data pack for the Sandia Engine Combustion Network (ECN)
small-bore diesel engine (GM / Opel 1.9 L derived, single-cylinder optical / metal
versions) from the ECN repository (ecn.sandia.gov/engines/small-bore-diesel-engine/)
for a 0-D / 1-D engine simulator. This engine is also the reference for the shared
spray / evaporation physics later used for petrol direct injection.

Common requirements:
- Return numbers with unit, exact source (URL + page/table/figure) and grade:
  D exact configuration, F family, R derived (show arithmetic), U not found.
  Never estimate silently.
- Report HARDWARE (physical dimensions, measured quantities). Values that exist only
  as simulation-model parameters go in a separate section labelled MODEL-DERIVED.
- Digitise plotted curves as (x, y) pairs at a stated resolution and say so.
- List every downloadable file (URL, name, contents) and extract the numeric tables
  requested.
- State clearly which hardware version (optical or all-metal, piston bowl, CR) each
  data set belongs to.
- Output a Markdown pack plus JSON {name, value, unit, grade, source}.

Inputs needed:
A. bore, stroke, rod length, CR, piston bowl geometry and volume, squish height.
B. valve diameters, intake and exhaust lift vs crank angle (arrays), measured valve
   flow coefficients and swirl ratio (vs lift).
C. intake / exhaust plumbing as run (surge tanks, pipe lengths / diameters,
   pressures), boost conditions.
D. Injector: holes x diameter, included angle, rail pressure, injection rate shape
   (numeric), SOI / duration commands and hydraulic delays; spray penetration, liquid
   length and spreading angle vs time (engine or matching ECN spray-vessel data, with
   the vessel conditions); fuel (composition, LHV, cetane, volatility / distillation).
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
and any related Chalmers / Volvo publications, for a 0-D / 1-D engine simulator.

Common requirements:
- Return numbers with unit, exact source (URL + page/table/figure) and grade:
  D exact configuration, F family, R derived (show arithmetic), U not found.
  Never estimate silently.
- Report HARDWARE (physical dimensions, measured quantities). Values that exist only
  as simulation-model parameters (e.g. calibrated VGT efficiency multipliers) go in a
  separate section labelled MODEL-DERIVED.
- Digitise plotted curves as (x, y) pairs at a stated resolution and say so.
- List every downloadable file and extract the numeric tables requested.
- Keep one engine build / rating; flag any mixing.
- Output a Markdown pack plus JSON {name, value, unit, grade, source}.

Inputs needed:
A. rod length, CR, firing order, bowl geometry if published.
B. valve diameters, lift profiles or timing; any engine-brake / VVA hardware.
C. Intake and exhaust manifold and pipe geometry, suitable for a 1-D pipe model
   (runner lengths / diameters, manifold volumes, pulse division); charge-air cooler
   (volume, pressure drop); EGR route and cooler; turbocharger: VGT type, compressor /
   turbine maps or measured operating points (pressure ratios, turbine inlet
   temperature, shaft speed).
D. Boost / VGT control: VGT vane or rack position per operating point, EGR valve
   position, any described control strategy (set-points, limits).
E. Injection: system, holes x diameter, rail pressure, SOI / quantity per point;
   fuel properties. Coolant / oil temperatures.

Outputs needed for each ESC point (and any full-load curve):
- speed, torque / BMEP, fuel flow, air flow, BSFC;
- boost, exhaust manifold pressure, turbine inlet temperature, EGR rate;
- cylinder pressure and burn rate (numeric);
- intake / exhaust runner pressure traces if published;
- motoring / FMEP data if available.
Also give the documented full-load torque / power curve of the same rating.
```

---

## S8 — Stage 1 search: handheld two-stroke < 100 cc

```
Stage 1 search (bounded): find two-stroke spark-ignition engines below 100 cc
(chainsaw, trimmer, leaf blower, model / small motorcycle, small outboard) for which
public sources give both simulator inputs and measured outputs. Purpose: validating a
two-stroke module (crankcase scavenging, port timing, tuned expansion chamber,
diaphragm carburettor) in a 0-D / 1-D engine simulator.

Report hardware (physical dimensions, measured quantities); values that exist only as
simulation-model parameters must be labelled MODEL-DERIVED.

For each candidate (aim for 2-3, do not pad), score availability as D (exact
configuration) / P (partial or related) / U (not found) for:
A geometry: bore, stroke, rod, crankcase volume / primary compression ratio,
  trapped compression ratio;
B ports: exhaust, transfer and intake (piston-port or reed) timings, widths, heights,
  areas vs crank angle, transfer-port angles;
C intake (carb, reed) and exhaust (expansion chamber / muffler) geometry suitable for
  a 1-D pipe model;
D carburettor metering (diaphragm type, jets, pulse-driven fuel pump), mixture, spark
  timing, fuel / oil ratio;
E thermal;
G full-load torque / power curve;
H fuel flow and AIR FLOW / delivery ratio / trapping and scavenging efficiency;
I cylinder and crankcase pressure traces;
J scavenging measurements (tracer gas, short-circuit losses), exhaust pressure traces.

Give source URLs (papers, theses, open datasets), data form (tables / plots / files)
and the main gaps. Conclude with a recommended candidate and a Stage-2 question.
```

---

## S9 — Stage 1 search: locomotive / marine medium-speed diesel

```
Stage 1 search (bounded): find medium-speed diesel engines (bore ~200-260 mm,
~900-1100 rpm; locomotive, marine or stationary; four-stroke e.g. ALCO 251, GE FDL,
MAN, Wartsila; and two-stroke e.g. EMD 567 / 645 / 710) for which public sources give
both simulator inputs and measured outputs. Purpose: validating heat transfer,
friction and combustion at large bore, the two-stroke module (uniflow scavenging) and
the scavenge-blower / supercharger module in a 0-D / 1-D engine simulator.

Report hardware (physical dimensions, measured quantities); values that exist only as
simulation-model parameters must be labelled MODEL-DERIVED.

For each candidate (aim for 2-3, do not pad), score availability as D / P / U for:
A geometry: bore, stroke, rod, CR, cylinder count, firing order;
B valves / ports and timing (exhaust valves and scavenge ports for uniflow
  two-strokes);
C intake / exhaust manifolds, air box, and the turbocharger / Roots blower
  arrangement: for EMD-type engines the gear-driven turbo with overrunning clutch or
  the Roots blower, with drive ratio, displacement and rotor dimensions;
D injection system (pump / unit injector), timing, nozzle data, fuel;
E thermal;
F turbo / blower data (maps, or measured boost, blower speed, turbine inlet
  temperature, and the clutch engagement / disengagement speed);
G load curve (power vs speed / notch);
H SFC and air flow at the same points;
I cylinder pressure (peak pressure at least);
J exhaust temperature, friction / motoring, auxiliary loads.

Prefer research papers, theses, service and maintenance manuals (e.g. MI-series),
classification-society or emissions certification data. Give URLs, data form and
gaps. Conclude with a recommended candidate per cycle type (four-stroke, two-stroke).
```

---

## S10 — Stage 1 search: supercharged petrol (automotive)

```
Stage 1 search (bounded): find mechanically supercharged automotive petrol engines
(Roots, twin-screw or centrifugal; e.g. GM L67 / LSA / LT4, Jaguar AJ-V8 / V6 SC,
Mercedes M111 Kompressor, Toyota 1ZZ / 2ZZ supercharged variants, Audi 3.0 TFSI SC,
Volvo T6 twin-charged) for which public sources give both simulator inputs and
measured outputs. Purpose: validating a supercharger module (mechanically driven
compressor, drive ratio, bypass valve, intercooler) in a 0-D / 1-D engine simulator.

Report hardware (physical dimensions, measured quantities); values that exist only as
simulation-model parameters must be labelled MODEL-DERIVED.

For each candidate (aim for 2-3, do not pad), score availability as D / P / U for:
A geometry: bore, stroke, rod, CR, layout;
B valvetrain: valve sizes, lift / timing, VVT;
C intake / exhaust geometry (1-D-model level), charge cooler volume and pressure drop;
D supercharger: type, displacement per revolution or impeller size, drive ratio,
  efficiency or performance map, bypass-valve behaviour, clutch (if any);
E fuel system (port / direct injection), spark, lambda;
G full-load torque / power curve;
H fuel and AIR FLOW at the same points;
I cylinder pressure;
J boost vs speed, charge temperature, supercharger drive power.

Prefer EPA / ANL / SAE benchmark data, university theses and OEM technical papers.
Give URLs, data form and gaps. Conclude with a recommended candidate.
```
