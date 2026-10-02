# Stage 2 packs P2-P5 — evaluation (2026-10-02)

Packs (user-supplied, this folder):
- `P2_Honda_GX160_validation_pack.md/.json`
- `P3_Kirloskar_TV1_AVL5402_validation_pack_bundle.zip`
- `ECN_P4_FINAL_validation_pack.zip`
- `P5_Volvo_D13_validation_pack.md/.json`

Judged against each engine's reference role (COVERAGE_MATRIX §3) and its [E] items. A missing essential narrows the role; it does not discard the engine.

## Verdict

| Pack | Role asked | Outcome | New grade |
|---|---|---|---|
| **P4 ECN small-bore diesel** | diesel combustion physics; spray / evaporation | **Met.** Native 0.25° pressure traces, measured injection-rate traces, fuel mass per cycle, intake pressure traces, O2, exact crank-slider / bowl / valve-lift / Cd data, Mie liquid length | **Physics reference** (closed-cycle diesel; spray for later DI) |
| **P3 AVL 5402 (Pawlak 2026)** | small-bore CR diesel with pressure | **Partial.** Exact geometry, rail pressure, pilot / main SOI, fuel, λ sweep, p(θ) + VHRR at 25 / 35 N m (−20..+30°). Nozzle F, rate shape U, rod / valves F | **Partial reference** (combustion phasing with pilot + main) |
| **P3 Kirloskar TV1 (Quadri 2018)** | small mechanical diesel with pressure | **Weak.** Pressure / NHRR trace at one point; pump internals, hole count and dynamic SOI U, so injection must be calibrated | **Calibrated engine** (adds little beyond Deere) |
| **P2 Honda GX160 (Ragan 2015)** | small SI; split charge vs efficiency | **Partial.** Same-test torque + BSFC vs speed (digitised, 316 g/kWh D at 2500), spark D, λ 0.95 at 2500 D, CR D, family cam-timing table and valve / carb dimensions. No full-load airflow; no usable pressure trace | **Calibrated engine with a held-out BSFC curve** |
| **P5 Volvo D13** | large-bore heat transfer / friction; turbo | **Not met.** No fuel flow, air flow, BSFC, boost, TIT, pressure or injection data; operating points are inputs, not results | **Not usable as a reference.** Slot 7 needs a replacement |

### P1 TCC-III (interim pack; Motored Full View archive still transferring)

| Item | Status |
|---|---|
| Geometry: bore 92, stroke 86, rod 231, CR 10 (effective 8 at IVC), TDC volume, crevices, flat pancake chamber | D |
| Valve events (EVC 12.8, IVC 240.8, EVO 484.8, IVO 712.8 CAD ATDCE), seat profile, port 25.4 mm | D |
| Valve lift arrays | F now (OpenFOAM tcc3 numeric files); D pending archive |
| Valve head diameter | U (seat 45° reference OD 29.9 mm bounds it) |
| Plenum volumes / ordered 1-D runner table | U pending (`.gtm` / CAD); drawings and plenum dimensions D |
| **Motored outputs, 3 conditions** (800 / 95, 1300 / 95, 1300 / 40 kPa): delivered air, IMEP, peak pressure and CAD, mean port pressures, wall temperature | **D scalars**; 0.5° pressure arrays pending |
| **Fired Full View** 1300 rpm / 40 kPa / φ 1 propane, MBT spark 342 CAD (18° BTDC), IMEP 323 kPa, COV 0.5 % | D scalars; arrays pending |
| **Spark Plug Region campaign**: CH4 / C3H8, φ 0.66-1.56, N2 dilution 0-19 %, measured O2 / N2 / fuel g/s, IMEP and COV per point | D scalars; CA10 / 50 / 90 and arrays pending |
| Fuel LHV | U in pack; propane / methane LHV are standard property data (S) |

**Verdict: physics reference, partially usable now.**
- Usable now: the motored scalars (breathing, compression, heat transfer and pumping at two MAPs and two speeds), and the fired φ and dilution sweep (IMEP vs mixture, a direct flame-model test).
- The pending archive completes 1-D pipe validation (port pressure arrays, runner geometry) and burn angles.

**Simulator needs for TCC-III:**
- fuel-specific laminar flame speed (Fuel::laminarBurningVelocity is gasoline-only; propane / methane Metghalchi-Keck / Gülder coefficients are published, S);
- wall temperatures as engine inputs (optical engine: quartz liner ~314 K outside wall vs the fixed 573 / 503 / 423 K);
- intake composition input for N2 dilution (shared with the ECN O2-dilution need).

## Data-quality corrections (do not use as labelled)

1. **P3 AVL derived BSFC / fuel flow is invalid.**
   - The pack derives BSFC from Fig. 10 "ηth" as if it were brake efficiency: 47.8 % at 10 N m (BMEP 2.46 bar), 42.4 % at 39 N m.
   - A 0.5 L single cylinder cannot reach 48 % brake efficiency at 2.5 bar BMEP (FMEP alone is ~1.5-2 bar). ηth is evidently indicated efficiency.
   - Use λ (D, vector-extracted) and p(θ) / VHRR. Fuel flow is U.
2. **P2 Aprinaldi (S2) airflow is unreliable.**
   - Derived AFR 6.3-9.7 (λ 0.43-0.67) is outside the rich limit of a running SI engine.
   - Anemometer velocities of 0.5-0.8 m/s are near instrument resolution. Exclude S2.
3. **P2 Kocakulak (S3)** is a worn engine with an irregular curve (7.7 / 8.4 / 7.1 Nm). Secondary only.
4. **P2 WEIMA pressure trace** is a clone at a generator load, read at 60° resolution. Not usable for burn validation.
5. **P5 exhaust mass flow** was multiplied by 0.75 by the authors. The exhaust *outlet* temperature is not the turbine inlet temperature.

## What P2 does answer (open question §0b 5a)

GX160 measured BSFC vs speed (S1): 344 / 329 / 324 / **316** / 320 / 335 / 350 g/kWh at 1000-4000 rpm.
- Brake efficiency peaks at 2500 and **falls** toward low speed.
- Torque is flat (9.85-10.2 N m) from 1500 to 3000.

The simulator's small-SI trend (efficiency rising toward low speed; GX390 brake-efficiency ratio 1.086 at 2000 vs 3600) can now be tested on a second engine with a measured efficiency curve.
- Caveat: λ is known only at 2500 (0.95), so mixture changes with speed partly confound BSFC.

## Simulator capabilities needed before P4 / P3 can be used fully

| Need | Why | Status |
|---|---|---|
| Prescribed (measured) injection-rate input | ECN supplies measured ROI; tier-2 input replacing the pump / rail model | missing |
| Multiple injections (pilot + main) | AVL and the ECN close-coupled study | missing (single event) |
| Intake O2 dilution / EGR composition input | ECN CDC9 19.7 % O2, LTC3 10 % | missing |
| DPRF58 fuel properties | ECN fuel; LHV U, compute from HMN + n-hexadecane literature values (R) | data step |

These are small, input-side additions consistent with the tiered-input schema (tier 2 component data). They are not new physics.

## Replacement for slot 7 (heavy-duty diesel)

D13 public data stop at the operating map. Search options (Stage 1):
- SwRI / EPA heavy-duty benchmark or GEM engine-map data;
- SAE papers on Cummins ISB / ISX or Caterpillar research engines with pressure traces;
- the medium-speed slot 9 for large bore.

Until then, large-bore physics rests on the published correlations' own validation plus the calibrated engines.
