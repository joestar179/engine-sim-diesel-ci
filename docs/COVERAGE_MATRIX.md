# Coverage matrix — engine classes, technologies and evidence

Controlling document for phases 2-5 of the agreed plan (CLAUDE.md §0f).
Draft 2026-10-02; to be agreed with the user.

It answers three questions for any engine a user might set up:
1. Can the simulator represent it? (technology status)
2. How good is the physics evidence at that size and technology? (references)
3. What grade of result should the user expect? (claim)

Status key: ✅ supported · ⚠️ partial / simplified · ❌ missing.
Evidence key:
- **R** physics reference (complete inputs + measured outputs);
- **C** calibrated engine(s) only;
- **—** none.

---

## 1. Architecture classes

| # | Class | Typical examples | Bore | Speed | Cooling | Status today | Evidence |
|---|---|---|---|---|---|---|---|
| 1 | Handheld two-stroke | leaf blower, chainsaw, trimmer (25-80 cc) | 32-50 mm | 7000-12000 | air | ❌ (no two-stroke cycle) | — |
| 2 | Small four-stroke single / twin | Honda GX, Kohler, Briggs (100-1000 cc) | 55-90 mm | 3000-3800 governed | air | ⚠️ curve shape fails (§0b 5a); carburettor simplified | C (GX390, Kohler) |
| 3 | Motorcycle / high-speed small | 125-1000 cc, 1-4 cyl | 50-80 mm | 8000-15000 | air / water | ⚠️ (wave tuning matters; no 1-D pipes) | — |
| 4 | Automotive NA petrol | 1.0-2.5 L I4, port or direct injection | 70-90 mm | 6000-7000 | water | ✅ full load; ⚠️ part-load scavenging; ⚠️ motoring / overrun | **R** (Mazda) |
| 5 | Automotive turbo petrol | 1.0-3.0 L TGDI | 70-90 mm | 6000-6500 | water | ⚠️ (turbo hardware ✅, no boost controller, no maps) | — |
| 6 | Automotive / V-engine large petrol | V6 / V8 3-6 L | 90-105 mm | 5500-6500 | water | ⚠️ (as class 4; no reference) | — |
| 7 | Small / medium diesel | 0.5-5 L industrial, mechanical pump | 80-110 mm | 1800-3000 | water / air | ✅ (NA and turbo) | C (Deere DF150 / TF250) |
| 8 | Common-rail turbo diesel | 1.5-3.0 L automotive | 80-95 mm | 4000-4500 | water | ⚠️ (common rail exists; VGT controller missing) | — |
| 9 | Heavy-duty diesel | 7-16 L truck / genset | 120-140 mm | 1800-2100 | water | ⚠️ (untested at size) | — |
| 10 | Locomotive / marine medium-speed | ALCO 251, EMD 645/710 (2-stroke), MAN / Wärtsilä | 200-260 mm | 900-1100 | water | ⚠️ ALCO four-stroke (too efficient, §0b 2b); ❌ EMD-type two-stroke | — (ALCO not set up) |

**Bore span to verify:** ~32 mm (class 1) to ~260 mm (class 10).
- Today the physics has one reference point, at 83.5 mm (Mazda).
- Calibrated evidence at 88-106 mm covers GX390, Kohler and Deere.

---

## 2. Technology modules

Each module has its own inputs, defaults and evidence, and is inert when absent. One physics set; no per-class physics switches.

| Area | Module | Status | Evidence | Needed for classes | Notes |
|---|---|---|---|---|---|
| Cycle | Four-stroke | ✅ | R | 2-10 | |
| | **Two-stroke** (crankcase / uniflow scavenging, port timing, short-circuiting) | ❌ | — | 1, 3 (some), 10 (EMD) | Needs 1-D pipes for expansion chambers |
| Mixture / injection | Carburettor | ⚠️ fixed λ | C | 1, 2, 3 | Speed / airflow-dependent metering missing |
| | Port fuel injection | ✅ (premixed) | R | 4, 5, 6 | |
| | **Petrol direct injection** | ⚠️ treated as premixed | R (Mazda, as premixed) | 4, 5 | Charge cooling and stratification missing |
| | Diesel mechanical pump | ✅ | C | 7, 9, 10 | Pump rate is a calibration knob |
| | Common rail | ✅ | — | 8, 9 | Not validated |
| Combustion | SI two-zone flame | ✅ | R (MBT check) | 1-6 | Low-speed efficiency question open (§0b 5a) |
| | Diesel framework (ignition delay, jet mixing) | ✅ | C | 7-10 | No pressure-trace validation |
| | Knock | ❌ | — | 4-6 | Audio has a knock layer; no autoignition model |
| Heat transfer | Hohenberg, fixed wall temperatures | ⚠️ | R (one size) | all | Air vs water cooling not differentiated; size scaling unverified |
| Friction | PNH components | ⚠️ | R (Mazda motoring), C (Deere motoring) | all | Ball-bearing small engines and large engines unverified |
| Air path | Lumped plenum / runners, inertial columns | ⚠️ | R (full load) | all | Rigid-slug limit: motoring / overrun wrong |
| | **1-D wave dynamics** (intake / exhaust pipes) | ❌ | — | 1, 3, 4 (part load), 6, 10 | Fixes motoring / overrun and scavenging; prerequisite for two-stroke |
| | Turbocharger (generic V1, multi-group, wastegate / VGT / bypass hardware) | ✅ hardware | C (TF250) | 5, 8, 9, 10 | No maps (reduced-order) |
| | Boost / VGT controllers | ❌ | — | 5, 8, 9 | Architecture exposes the boundary |
| | **Supercharger** (mechanically driven; Roots / screw / centrifugal; scavenge blowers) | ❌ | — | 4-6 (common on petrol automotive), 1 / 10 (two-stroke scavenge blowers) | Reuses the compressor model; designed after 1-D pipes and two-stroke |
| | Charge-air cooler | ✅ | C | 5, 8-10 | |
| | EGR | ❌ | — | 8, 9 | Architecture exposes branches |
| Control | Mechanical governor (droop, k_p, crank limit) | ✅ | C | 2, 7, 9, 10 | |
| | VVT schedule | ✅ | R (Mazda phases) | 4, 5, 6 | |
| | Smoke limiter | ✅ | C | 7-10 | |
| | ECU (λ / spark maps) | ⚠️ inputs only | R | 4-6, 8 | |

---

## 3. Physics references: sourcing slots

Stage-1 slots 1-7 (`docs/reference/validation_sourcing_stage1.md`) plus two new slots.

| Slot | Role | Preferred candidate | Fills | Status |
|---|---|---|---|---|
| 1a | Fundamental SI (pressure traces, valve motion, gas exchange) | UM TCC-III (92 mm bore) | flame and heat transfer at a second size; gas exchange | Stage 2 now |
| 1b | Small SI with brake performance + BSFC + pressure | Honda GX160 (68 mm) | **directly tests the §0b 5a low-speed efficiency question** | Stage 2 now |
| 2 | Automotive NA SI | Mazda 2.0 SKYACTIV-G | — | ✅ in use (R) |
| 3 | Turbo SI | Ford 1.6 EcoBoost | turbo SI | later |
| 4 | V-engine SI | (weak slot; do not pad) | — | later |
| 5 | Small mechanical diesel | Kirloskar TV1 (87.5 mm); AVL 5402 | diesel pressure traces, small bore | Stage 2 now |
| 6 | Common-rail diesel | GM 1.9 / ECN small-bore (82 mm) | diesel combustion physics (spray, pressure) | Stage 2 now |
| 7 | Heavy-duty diesel | Volvo D13 (131 mm) | heat transfer and friction at a large bore | Stage 2 now |
| **8 (new)** | Handheld two-stroke < 100 cc | to be searched (university chainsaw / trimmer studies with pressure, port timing, scavenging data) | two-stroke module validation; smallest bore | Stage 1 search |
| **9 (new)** | Locomotive / marine medium-speed | to be searched (ALCO / EMD / MAN research or manuals with performance + SFC; ideally pressure) | largest bore; ALCO efficiency question (§0b 2b) | Stage 1 search |

**Minimum set before calling the physics general:**
- SI at two sizes beyond the Mazda: TCC-III and GX160;
- diesel at two sizes: ECN small-bore and Volvo D13;
- one two-stroke reference before claiming class 1.

---

## 4. Tiered inputs (spec sheet = minimum)

| Tier | Inputs | Replaces | Claim |
|---|---|---|---|
| 0 | Spec sheet: layout, bore, stroke, CR, rated power / speed, fuel system, cooling, aspiration | — | Plausible (defaults) or calibrated (with a documented curve) |
| 1 | Geometry: valve sizes and lift, cam card, runner / exhaust lengths and diameters, turbo model, injector holes | Layer-2 defaults | as tier 0, fewer defaults |
| 2 | Component data: flow-bench curves, measured lift profiles, pump / injector rate, turbo maps | calibration knobs (e.g. measured valve flow removes `port_cd`) | as tier 0, fewer knobs |
| 3 | Test data: airflow, BSFC map, EGT, boost, motoring, pressure traces | — (adds held-out checks) | can reach "validated" |

Rules:
- Every input carries a source grade (S/D/F/R/C/A).
- User data always overrides defaults.
- A calibration knob exists only for an input the user has not supplied.
- The engine's claim follows from what was supplied.

**Implementation gap:** only the SI generator (`tools/engine_setup`) works this way. Diesel and turbo engines are hand-written MR scripts (phase 4).

---

## 5. Phase-5 order (capability gaps), agreed 2026-10-02

The order is set by **impact and by rework risk**: what each gap changes underneath, and what would have to be redone if it came later. Implementation cost does not set the order.

| # | Item | What it changes underneath | Why here | Validation |
|---|---|---|---|---|
| 1 | **1-D intake / exhaust pipes** | The gas-path foundation (runner / primary / collector / duct elements; main exhaust sound source) | Affects every class. Fixes a known error (motoring / overrun +30 %, overrun sound). Prerequisite for two-stroke pipes, carburettor metering under pulsating flow, supercharger ducting and turbo pulse behaviour. Replaces runner / primary elements **inside** the locked turbo topology (CLAUDE.md §1), never the topology | Mazda motoring and part load; TCC-III gas exchange |
| 2 | **Two-stroke cycle** | Cylinder-level assumptions: piston-controlled ports, the crankcase as a gas volume, scavenging, firing every revolution; friction (no valvetrain), injection scheduling, governor timing, audio firing frequency, setup library | Opens classes 1 and 10 (EMD) and outboard / motorcycle two-strokes, which cannot be simulated today. Settling the cycle-level design early prevents rework of everything built after it | Slot 8; slot 9 (two-stroke candidate) |
| 3 | **Petrol direct injection** (charge cooling, stratification, evaporation) | In-cylinder mixing | Most modern automotive petrol engines. Designed together with the diesel spray model (shared spray / evaporation physics), after the ECN diesel reference | Mazda (DI), slot 3; ECN for the shared spray physics |
| 4 | **Boost / VGT control** | Controller policies on the existing wastegate / VGT / bypass hardware | Needed for most modern turbo engines (classes 5, 8, 9) to run realistically | Slot 3 (turbo SI), slot 7 (D13 VGT) |
| 5 | **Carburettor metering** (float and diaphragm) | Mixture vs airflow under pulsating flow | After 1-D pipes (venturi flow pulses) and two-stroke (diaphragm carburettors driven by crankcase pulses), so one design covers both | GX160 (slot 1b); slot 8 |
| 6 | **Supercharger** (Roots / screw / centrifugal; scavenge blowers) | Mechanically driven compressor on the gas path | Common on petrol automotive engines. Placed last for implementation reasons: it depends on 1-D pipes (ducting) and on the two-stroke design (gear-driven scavenge blowers, EMD's gear-driven turbo with an overrunning clutch), so it is designed once for automotive superchargers and blowers alike | Reference to be sourced (supercharged petrol; slot 9 two-stroke) |

**Design rule for inputs (applies to phase 4 and every module):** inputs describe **hardware**, never model internals.
- Pipe lengths / diameters / tapers, port timings and areas, blower displacement and drive ratio are physical, so they survive the move from lumped to 1-D models.
- Model artefacts (e.g. a lumped "runner flow rating") are not schema inputs; where one is needed it is derived from hardware inputs or is a declared calibration knob.

Separate open physics questions (not capability gaps), to be tested as the references arrive:
- small-SI low-speed efficiency (§0b 5a) → GX160;
- heat-transfer and friction size scaling → TCC-III, Volvo D13, slot 9;
- ALCO efficiency (§0b 2b) → slot 9.
