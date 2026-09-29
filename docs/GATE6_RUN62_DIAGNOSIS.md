# Gate 6 Run 62 — read-only diagnosis and diagnostic-gate plan

Status: diagnosis complete; Gate 6B authorized by the user; Gate 6A proposed,
not authorized.

No production, MR, calibration or test file was changed to produce this report.

## 1. Verified state

- Remote HEAD `43969f2aad234ace4efa835e18e30dd36e677743`; implementation commit
  `71fff99ee8b78666107f006028c223933ab41f55` directly beneath it.
- Run 62 (`36584792845`) tested `71fff99`; artifact
  `engine-sim-gate6-alco-251b-loaded-transient` (ID `11041298653`) digest
  `664eedb81b9b94253a0bf4cb5efc84b167996107d609e6e4e73266ba1707c389` matches
  the handover.
- CI seed SHA-256 `47d987cfeb1f90b53fa203eda2a1a669cd660b5838a9c49b83389d1c1ba8fd23`
  verified by local decode.
- The artifact archive itself could not be downloaded from the diagnosing
  environment (blob host blocked by its network policy). The final 400 lines of
  the job log (build tail, all three Gate 6 tests, artifact upload) were read
  instead.

No discrepancy with the committed handover was found. Minor observations:

- `alco_16_251b_native.mr` lines 334-339 still describe the stock governor
  acting on the intake; the configured `diesel_governor` commands the fuel rack.
- `Engine::resetFuelConsumption()` resets the direct-injected counter but not
  the direct-burned counter. Gate 6 measures per-window deltas, so this does not
  affect Run 62.
- `CompressionIgnitionModel`, `DieselGovernor`, `FuelRackGovernorModel` and
  `GasSystem::reactFuel` are installed by the seed-owned
  `tools/apply_ce_enhancement.py`, not by the readable V1 overlay.

## 2. Primary evidence gap

`alco_251b_loaded_transient_validation.cpp` throws at the first failed
assertion and prints only the `GATE6_FAIL` line. The numeric low/high/release
values are printed only on success. Run 62 therefore contains no numeric
telemetry for either failure, and the artifact XML cannot contain more than the
single reason line for those processes.

## 3. Control report — rack did not retreat after release

Governor model (seed `src/fuel_rack_governor_model.cpp`):

```text
e          = target^2 - speed^2                      (rad^2/s^2)
rackRate  += dt*k_s*e - dt*k_d*rackRate, clamped to [min_v, max_v]
rack      += rackRate*dt, clamped to [0, 1]
output     = rack^gamma
```

There is no proportional return toward a rack position; the rack moves only
while a speed error exists.

16-251B configuration: `min_speed` 400 rpm, `max_speed` 1000 rpm, `k_s` 0.008,
`k_d` 120, `min_v`/`max_v` ±2, `gamma` 1.5.

Fixture (Run 62): dyno hold at 600 rpm; `LowCommand = (600-400)/(1000-400)`,
which maps the governor target to exactly the held speed.

Static prediction:

- High window: target 1000 rpm, held 600 rpm, `e ≈ 7018`; steady `rackRate ≈
  k_s*e/k_d ≈ 0.47/s` (below `max_v`). The rack ramps and clamps at 1; the
  window mean is below 1.
- Release window: target 600 rpm equals held speed, `e ≈ 0`. `rackRate` decays
  with time constant `1/k_d ≈ 8.3 ms` and the rack stays at 1. A 1 rpm hold
  error would move it only about 0.004 over the 5 s window.
- Therefore `mean(release) ≈ 1.0 >= mean(high)`, reproducing the exact Run 62
  signature. Classic integral wind-up is structurally limited because both
  `rack` and `rackRate` are clamped and `rackRate` is strongly damped.

Leading hypothesis: invalid fixture assumption through command mapping — the
release command targets the dyno-held speed, so an isochronous governor has no
error to act on. This is a static prediction, not evidence.

Missing evidence:

- actual held speed and its sign (the starter uses `-starterSpeed`; the test
  sets the dyno to `+600 rpm`; the upstream dynamometer sign convention is not
  in the overlay or seed; the test accumulates `rpmTime` but never reports it);
- target speed, error, rack and internal rack rate through every phase;
- a probe with a genuinely negative error to separate fixture assumption from
  release logic or persistent state.

## 4. Combustion report — burned fuel did not increase

Direct-injection events start at ignition-module events with
`0.8 g * rack` per event. Burning proceeds only while cylinder temperature is at
least 550 K and pressure at least 1 MPa, and `reactFuel` is oxygen-limited.
Gate 5 never asserted burned fuel > 0, so no gate has shown any 16-251B
combustion.

| Hypothesis | Discriminating evidence |
|---|---|
| C1 no combustion (burned = 0 both windows): thresholds never met, or injection phased away from TDC | lit state, event-window peak T/P, cylinder volume at injection start / clearance volume |
| C2 oxygen-limited in both windows (full rack needs about 85 mmol O2; naturally aspirated trapped O2 is at most about 100 mmol) | residual O2 and fuel at exhaust-valve opening, trapped air |
| C3 burn truncated when T/P fall below thresholds during expansion | lit ending with both O2 and fuel remaining; T/P at that moment |
| C4 run-to-run variation | printed values; Run 62 not reproducing |

All required quantities are reachable through existing public accessors
(`m_system`, `isLit()`, `m_nBurntFuel`, `getTrappedAirMoles()`,
`getCompressionIgnitionModel()->parameters()`).

## 5. Diagnostic gates

Common rules: new single-purpose test file, own executable and one CTest entry,
own mutually exclusive CI mode; official v0.1.14a SI null first; Run 62
fixture sequence reproduced exactly; the gate passes on evidence completeness
and reports a classification rather than asserting physics; no production C++,
MR or calibration change; production templates pinned to their `71fff99`
content by the readable application tool.

### Gate 6B — governor/control observability (authorized)

- Mode: `ALCO_251B_GOVERNOR_OBSERVABILITY_ONLY=1`.
- Files: `patches/forced_induction_v1/test/alco_251b_governor_observability.cpp`,
  `patches/forced_induction_v1/CMakeLists.txt`,
  `tools/apply_forced_induction_v1.py`, `tools/enable_gate2_compile.py`,
  `.github/workflows/windows-ci.yml`, `docs/FAILURE_ATTEMPT_LOG.md`.
- Telemetry: commanded and applied speed control, target speed, held speed and
  signed crank speed, error, output rack, internal rack, reconstructed rack
  rate, saturation time, and a shadow of the governor model driven by the
  observed speed (divergence exposes hidden state or parameter mismatch).
  100 Hz trace plus full-rate capture for 50 ms after each command change.
- Probe: after the unchanged release window, command 0.0 (target 400 rpm, held
  600 rpm) for 300 frames.
- Classifications: `FIXTURE_INVALID`, `COMMAND_MAPPING`,
  `RUN62_NOT_REPRODUCED`, `FIXTURE_ASSUMPTION`, `PERSISTENT_STATE`,
  `RELEASE_LOGIC_OR_WINDUP`, `UNCLASSIFIED`.
- Failure signature: `diagnostic | 16-251B governor evidence incomplete |
  test-harness layer`; two attempts.

### Gate 6A — combustion observability (proposed, not authorized)

- Mode: `ALCO_251B_COMBUSTION_OBSERVABILITY_ONLY=1`.
- Telemetry per window: held speed, rack, fuel per event, injected and burned
  mass and ratio. Per injection event: cylinder, volume ratio at injection
  start, T/P at start and event peak, lit, burned moles, residual O2 and fuel at
  exhaust-valve opening, trapped air.
- Classifications: C1 (subtyped), C2, C3, C4, fixture invalid.
- Failure signature: `diagnostic | 16-251B combustion evidence incomplete |
  test-harness layer`; two attempts.

Order: 6B first, then 6A, as separate CI runs. A repair in either layer is
proposed only after its classification exists.
