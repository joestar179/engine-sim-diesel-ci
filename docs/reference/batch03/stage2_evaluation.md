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
| **P5 Volvo D13** (re-checked with `_COMPLETE` version: no new numbers; fuel / air / exhaust / cylinder pressure measured but unpublished) | large-bore heat transfer / friction; turbo | **Not met.** No fuel flow, air flow, BSFC, boost, TIT, pressure or injection data; operating points are inputs, not results | **Not usable as a reference.** Slot 7 needs a replacement |

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

## S9 medium-speed diesel (EMD 16-710G3B, Wärtsilä 8L26)

| Engine | Outcome | Grade |
|---|---|---|
| EMD 16-710G3B (two-stroke, 230 mm) | Geometry D; scavenge-port timing D; one rated point (power, air 6.0 kg/s, turbo speed, TIT / TOT, pmax 108 bar). **No fuel at any load**; port dimensions U (scavenging cannot be validated); EVC transcription contradicts the event sequence (not used) | Rated-point consistency only |
| Wärtsilä 8L26 (four-stroke, 260 mm) | Declared BSFC at 4 loads (85 % guaranteed), air and exhaust flow, exhaust temperature, full heat-rejection split. Rod U, CR family-only, valve / VIVC timing family-only, boost and TIT U, no pressure trace | Calibrated-grade large-bore thermal / system check |

- Neither meets the minimum (same-point fuel + air + output, plus pressure traces or turbo boundaries).
- The ALCO 251 and GE FDL named in the prompt were not evaluated.

## S10 supercharged petrol (GM LSA, VW 1.4 TSI Twincharger BLG)

| Engine | Outcome | Grade |
|---|---|---|
| GM LSA 6.2 (Roots) | Displacement 1.9 L/rev D, drive 2.6 F, no clutch, peak boost 9 psi D, torque curve (F, marine variant), 6 airflow points (F). Fuel, map, bypass, charge temperature, drive power U | Partial: blower flow model |
| VW BLG Twincharger | Ratio 5:1, 17 500 rpm, clutch, bypass flap, operating ranges (D); SC and combined pressure-ratio vs speed (digitised); torque curve. **SC displacement U, air and fuel U** | Partial: series SC + turbo topology / control |

**LSA airflow table is not consistent with its torque table.** Air per cycle per unit torque is 0.0176 / 0.0153 / 0.0169 / 0.0147 / 0.0196 g/(lb-ft) at 2000-5200 rpm. At 4400 rpm the air is below that at 3600 while torque is higher. GM labels it "marine-engine airflow requirements". Use it as a rough check only, not a held-out target.

Implied Roots volumetric efficiency (air / (1.9 L × SC speed × 1.18 kg/m³)): ~0.68 at 2000 rising to ~0.87 at 5200 engine rpm. Plausible, R.

Gap prompts: `stage2_gap_prompts.md` G6 (S9) and G7 (S10).

## Extended packs (after gap prompts G2-G4), 2026-10-02

### P4 ECN extended: essential gaps closed for the CDC9 cases

| Item | Result |
|---|---|
| DPRF58 LHV / density / stoichiometric AFR | 43.9 MJ/kg; 682 kg/m3 at 436 K; 14.90 dry air (R from the ECN fuels table / formula) |
| CDC9 intake composition and temperature | O2 19.7 / N2 79.2 / CO2 1.1 / H2O 0 vol %, dry synthetic charge (D); 353 K (D) |
| Exhaust back-pressure | 145.7 kPa abs constant (D, CDC9); U for LTC3 and close-coupled |
| LTC3 composition | 10 % O2, 9 % CO2 (F) |
| Valve head OD, fuel-off motored trace, measured wall temperatures, 1-D runners | still U (I / N); inert non-combusting traces remain the F motored substitute |

**Verdict:** a complete closed-cycle diesel physics reference for the six CDC9 bowl-study cases. Use these first; LTC3 and close-coupled are secondary.

### P3 extended

- The AVL ηth-derived BSFC / fuel flow is **retracted** in the pack itself (ηth undefined in the source). AVL fuel and air flow remain U. The paper states the data are available from the corresponding author on request.
- AVL spray angle 162° (F, same group). Hole count x diameter still U.
- TV1 Çakmak 2023 (3.5 kW, same rig type): full-load BSFC 0.207 kg/kWh implies ~41 % brake efficiency for a 0.66 L single at 1500 rpm. **Implausible**; do not use. Its air flow is measured but unpublished.
- No change to grades: AVL partial reference, TV1 calibrated.

### P2 extended

| Source | Content | Assessment |
|---|---|---|
| H1 Honda official GX160 sheet (CR 9.0, SAE J1349) | 10.1 / 10.3 / 10.1 / 9.55 N m at 2000 / 2500 / 3000 / 3600; 3.6 kW at 3600; 1.4 L/h at 2.9 kW continuous (3600) | Good. Consistent with S1 Ragan. Use as the manufacturer curve |
| S5 Çelebi 2022 (stock, full load) | torque 6.3-7.2 N m; BSFC 418-493 g/kWh; λ range 0.814-1.000 (not per speed); EGT | **Low-output rig:** ~70 % of rated torque, brake efficiency 17-20 %. Not a reference |
| S6 Torres 2024 (EFI, E100, CR 7.44 / 9.44, WOT, λ 1 ± 0.015, measured torque + fuel) | air derived = fuel x 8.91 | **Internally implausible**, see below |

S6 consistency check (CR 9.44, 3500 rpm): brake power 4.14 kW against fuel energy 0.387 g/s x 26.9 MJ/kg = 10.41 kW, i.e. brake efficiency **40 %**. At CR 7.44 / 2000 rpm it is 35 %.
- Both are far above what a 163 cc air-cooled SI engine can reach. The ideal Otto efficiency at CR 9.44 is ~49 %; 40 % brake would need ~80 % of ideal after friction.
- The derived air (λ 1) gives volumetric efficiency of only 0.63-0.68, yet torque is above the stock engine's.
- Both anomalies point to fuel flow reading ~25-30 % low (EFI return / gravimetric set-up). The derived air flow is therefore not usable until the raw Mendeley data explain it.

**GX160 status:** still a calibrated engine with a BSFC curve (S1) and a manufacturer torque curve (H1). The airflow gap stays open.

### P2 S6 Torres 2024: raw-data follow-up

Raw file: `Torres2024_GX160_EFI_ethanol_supplementary-data.xlsx` (Mendeley yvb7khhbrj v1, sha256 d08c4e98...). Sheets Raw (360 samples), Average, Std.Dev.

1. The authors' own "Efficiency" column is 34-40 % (W0, CR 7.44 / 9.44). The implausible efficiency is in the source, not an extraction error.
2. Fuel consumption is logged at 0.01 g/s resolution: ±4 % per sample at 2000 rpm.
3. **The emissions contradict the stated λ 1.000 ± 0.015.**
   - Dry O2 is 12.7-16.6 % with CO2 7.6-11.9 % and CO 0.5-0.9 %.
   - O2 + CO2 = 23-27 %. Ethanol-air combustion cannot exceed ~20 % dry, even with sample dilution by air. The analyser data are therefore unreliable.
   - Taken at face value, the O2 alone would mean λ well above 2.
4. With λ 1 and a realistic brake efficiency (~27 %), fuel would be ~0.29 g/s at 2000 rpm vs 0.227 logged (22-28 % low), and VE would be ~0.81 (plausible). That is consistent with a fuel-flow under-reading.

**Decision:** S6 is not validation-grade (fuel flow and emissions internally inconsistent). Torque values may still be usable as a modified-build brake check. The GX160 airflow gap stays open.

### S10 extended (after G7)

**LSA:**
- No same-test WOT air / fuel / boost curve. All [E] items still U.
- New: stock drive ratio 2.56 (exact build, secondary; supersedes 2.60 F); cam P/N 12623064 (198 / 216° @ 0.050, 0.480 in lift, LSA 122.5°, D); OEM-family bypass control data (closed by spring, opening ~10.2-33.9 kPa vacuum, ~4 psi boost trim); GM marine-family fuel-flow table.
- The marine air and fuel tables give a smooth apparent AFR (14.7 → 10.4) including at 4400 rpm. The 4400 rpm dip is therefore common to both tables: these are marine requirement conditions, not WOT. Still not same-test with the J2723 torque table.
- **Best lead:** SAE CPGM2_09CADCTSV (J1349 Certified Power Engine Data, Level 2, exact 2009 CTS-V LSA). According to its catalogue description it contains all J2723 measured test parameters. It is a paid SAE document.

**BLG:**
- Supercharger candidate Eaton M24, 0.390 L/rev (F, weak provenance). Consistent with 5:1 and 17 500 rpm at 3500 engine rpm. Usable only as an F initializer.
- Air / fuel flow still U.

**Verdict unchanged:** partial references (LSA blower flow and bypass logic; BLG twincharger control). Acquiring the SAE certified-power document is the single step that could make the LSA a quantitative reference.
