# Stage 2 gap prompts (2026-10-02)

Follow-up requests for the packs in this folder. Each prompt targets only the gaps that matter for that engine's reference role (`stage2_evaluation.md`). Each is self-contained.

Common block (repeated in each prompt):
- numbers with unit, exact source (URL + page / table / figure) and grade (D / F / R / U);
- hardware separate from MODEL-DERIVED values;
- digitise plots as (x, y) pairs and say so;
- one test configuration, no silent mixing;
- Markdown + JSON {name, value, unit, grade, importance, source};
- **return the full pack (previous content plus additions); do not summarise or compress earlier tables.**

---

## G1 — TCC-III: archive extraction (when the Motored Full View archive is available)

```
Update the existing University of Michigan TCC-III validation pack
(TCCIII_validation_pack_current) by extracting from the downloaded TCC-III archives
(Motored Full View, Fired Full View, Fired Spark Plug Region). Return the FULL pack
with all previous content plus the additions; do not summarise earlier tables.

Common requirements: numbers with unit, exact source (archive path / file / sheet /
cell, or URL + page) and grade (D exact, F family, R derived with arithmetic, U not
found); hardware separate from MODEL-DERIVED (e.g. GT-Power calibration values);
keep each test separate; output Markdown + JSON {name, value, unit, grade,
importance, source}. Numeric arrays go in CSV files at native resolution.

Extract:
[E] TCCIII_Valve_Lifts: intake and exhaust lift vs crank angle (full arrays, native
    resolution, stating the crank-angle convention and any lash).
[E] Valve head diameters (and stem diameters) from the GT-Power model, CAD or
    drawings.
[E] From the GT-Power .gtm / .gdx: every intake and exhaust flow element from the
    plenum boundaries to the valves (pipe lengths, diameters in / out, bend radii,
    volumes of plenums / flow-splits), the throttle element and the valve flow
    coefficients (Cd or flow vs lift, forward and reverse). Label any calibrated
    multipliers as MODEL-DERIVED.
[E] Motored pressure workbooks for tests S_2014_05_20_02 (800 rpm / 95 kPa),
    S_2014_05_20_01 (1300 / 95) and S_2013_10_24_01 (1300 / 40): Ensemble_Average of
    P_IntakePlenIn, P_IntkPort, P_Cyl, P_ExhPort, P_ExhPlenOut at 0.5 CAD (CSV), plus
    the Test Info / summary values (intake temperature, delivered air flow, IMEP).
[E] Fired Full View test S_2014_02_13_02: the same five 0.5-CAD ensemble channels,
    intake temperature, delivered air and fuel flow, IMEP, COV, and per-cycle
    CA10 / 50 / 90 statistics from Per_Cycle_Data.
[E] Spark Plug Region consolidated workbooks: for the 11 operating conditions,
    ensemble cylinder pressure and apparent heat release at 0.5 CAD (CSV), and per-test
    mean CA10, CA50, CA90, Burn0010, Burn1090, IMEP, COV, intake temperature.
[I] Fuel specification used (propane / methane grade and purity); if not stated,
    mark U (standard property data will be applied separately).
[I] Any measured wall / head / piston temperatures.
[N] Complete archive file manifest (path, size, contents).
```

---

## G2 — Honda GX160: full-load air flow and the operating point of the pressure trace

```
Extend the Honda GX160 validation pack (P2_Honda_GX160_validation_pack). Return the
FULL pack with previous content plus additions; do not summarise earlier tables.

Common requirements: numbers with unit, exact source (URL + page/table/figure) and
grade (D exact, F family, R derived with arithmetic, U not found); hardware separate
from MODEL-DERIVED; digitise plots as (x, y) pairs and say so; one test set-up per
study, no silent merging; Markdown + JSON {name, value, unit, grade, importance,
source}.

Gaps to close, in priority order:
[E] A GX160 study with FULL-LOAD (wide-open throttle) torque, fuel flow AND air flow
    on the same test set-up, across speed. Air flow may be measured directly (MAF,
    orifice / air-box manometer, laminar-flow element) OR derived from a measured
    lambda / AFR together with measured fuel flow (state which). Good places to look:
    GX160 / GX200-class alternative-fuel conversion studies (LPG, CNG, ethanol,
    hydrogen, biogas) that report a gasoline baseline with lambda and fuel flow;
    university dynamometer theses and lab reports (e.g. Liverpool MECH217 results);
    carburettor / intake-tuning studies.
[E] Full-load lambda / AFR vs speed for the stock carburettor (any GX160 study, same
    set-up as its torque data).
[E] Measured exhaust back-pressure with the stock muffler at full load (any GX160 /
    GX200 study).
[I] The engine speed and load of the measured cylinder-pressure traces in Ragan et
    al. 2015 (Fig. 3), from the full paper, the authors' related publications or
    thesis; or any other exact-Honda GX160 pressure trace at a stated speed / load
    (numeric or digitised).
[I] Motoring / friction (FMEP) data for the GX160 or GX200.
[I] Honda's official GX160 performance curve (torque / power vs speed, SAE J1349),
    digitised, with its rating basis.
```

---

## G3 — AVL 5402 (Pawlak) and Kirloskar TV1: injection hardware, efficiency definition, air flow

```
Extend the P3 Kirloskar TV1 / AVL 5402 validation pack. Return the FULL pack with
previous content plus additions; do not summarise earlier tables.

Common requirements: numbers with unit, exact source (URL + page/table/figure) and
grade (D exact, F family, R derived with arithmetic, U not found); hardware separate
from MODEL-DERIVED; digitise plots as (x, y) pairs and say so; one engine build and
test set-up per study, no silent mixing; Markdown + JSON {name, value, unit, grade,
importance, source}.

AVL 5402, Pawlak et al. 2026 (doi 10.1002/ese3.70482) build:
[E] Definition of the "thermal efficiency" in Fig. 10: brake or indicated (gross /
    net)? Quote the defining equation or text. (The pack derived BSFC from it as if
    brake; 48 % at 2.5 bar BMEP indicates it is not brake.)
[E] Measured fuel flow per load point (Cori-Flow) or BSFC, from the paper, its
    supplementary data or the same group's other papers on this rig.
[E] Injector nozzle: holes x diameter and spray angle, confirmed from a primary
    source (Bosch catalogue / injector datasheet / the group's earlier papers on this
    AVL 5402); connecting-rod length for this build.
[I] Measured injection-rate shape for this injector at ~750 bar (the group's work or
    literature on the same Bosch injector), numeric or digitised.
[I] Air flow per load point (or the lambda definition plus fuel flow).
[I] IMEP and full-cycle cylinder pressure at 25 / 35 N m; exhaust temperature.

Kirloskar TV1:
[E] One single TV1 study that publishes, on the SAME set-up: cylinder pressure trace
    (numeric or plotted), BSFC / fuel flow, AIR FLOW (TV1 rigs usually log air-box
    manometer air flow), injection timing and injection pressure, at stated load.
    Prefer studies with the standard 3.5 kW / 1500 rpm rig and a stated nozzle.
[E] Injection hardware for that build: nozzle holes x diameter, MICO pump element
    plunger diameter, dynamic start of injection or measured line-pressure / needle
    lift.
[I] Exhaust temperature and motoring / friction data for the same rig.
```

---

## G4 — ECN small-bore diesel: remaining boundary and fuel data

```
Extend the ECN small-bore diesel validation pack (ECN_P4_FINAL_validation_pack).
Return the FULL pack with previous content plus additions; do not summarise earlier
tables or drop the existing CSV files.

Common requirements: numbers with unit, exact source (URL / file / sheet or paper +
page/table) and grade (D exact, F family, R derived with arithmetic, U not found);
hardware separate from MODEL-DERIVED (e.g. prescribed CFD wall temperatures); keep
bowl-study, close-coupled and optical / metal versions separate; Markdown + JSON
{name, value, unit, grade, importance, source}.

Gaps:
[E] Fuel properties of DPRF58 (58 vol% 2,2,4,4,6,8,8-heptamethylnonane + 42 vol%
    n-hexadecane): density, lower heating value, H/C ratio, stoichiometric AFR,
    distillation / volatility. Use ECN / Sandia documentation first; otherwise
    component property data (NIST / DIPPR) with the mixing arithmetic shown (R).
[E] Exhaust back-pressure (mean and, if available, crank-angle trace) for the bowl-
    study CDC9 / LTC3 fired cases and the close-coupled cases.
[E] Intake charge composition for each case: O2, N2, CO2, H2O fractions (how the
    dilution / EGR was simulated: N2 only or N2 + CO2), and intake temperature at IVC
    or in the runner for the bowl-study cases (D rather than F).
[I] Physical intake and exhaust valve head diameters (distinct from the 24 / 22 mm
    flow-reference diameters), from ECN / Sandia drawings or papers.
[I] A fuel-off motored cylinder-pressure trace at the same intake conditions
    (pressure, temperature, composition) as the fired cases, or the closest
    documented one.
[I] Any measured wall / head / piston temperatures for the metal engine.
[N] 1-D intake / exhaust runner and surge-tank dimensions reduced from the ECN STEP
    geometry (lengths, diameters, volumes).
```

---

## G5 — Heavy-duty diesel: replacement search for slot 7 (D13 not usable)

```
Stage 1 search (bounded): the Volvo D13 public data stop at the speed-torque map
(fuel flow, air flow, boost and cylinder pressure were measured but never published).
Find replacement heavy-duty diesel engines (bore ~110-140 mm, 6-16 L, or single-
cylinder research versions of such engines) with public numeric data for validating
heat transfer, friction, combustion and turbocharging at large bore in a 0-D / 1-D
engine simulator.

Report hardware (physical dimensions, measured quantities); values that exist only as
simulation-model parameters must be labelled MODEL-DERIVED.

Candidate areas (verify, do not assume): university heavy-duty research engines with
extensive publications (e.g. Purdue Cummins ISB / ISX variable-valve-actuation work,
Lund / KTH Scania single-cylinder engines, Wisconsin / Sandia heavy-duty optical
engines, Chalmers / Volvo single-cylinder), EPA / SwRI heavy-duty benchmark or GEM
engine-map data, SAE papers with complete operating-point tables.

For each candidate (aim for 2-3, do not pad), score availability as D / P / U for:
A geometry (bore, stroke, rod, CR, bowl); B valves and timing; C intake / exhaust /
turbo / EGR hardware; D injection (system, nozzle holes x diameter, pressure, SOI,
quantity per point); E thermal; F turbo operating data (boost, exhaust manifold
pressure, turbine inlet temperature, maps); G torque / load points; H fuel flow, air
flow and BSFC at the same points; I numeric cylinder pressure; J motoring / FMEP,
EGR rate.

The minimum useful pack is: same-point fuel flow + air flow + brake (or indicated)
output, plus either cylinder pressure or turbo boundary data (boost, exhaust
pressure, turbine inlet temperature). Give URLs, data form and gaps. Conclude with a
recommended candidate.
```
