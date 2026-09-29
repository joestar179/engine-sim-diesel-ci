# Failure Attempt Log

This log is part of the active Failure-Loop Guard. Repeated failures are recorded by signature so the same speculative repair cannot be retried indefinitely.

## Active incident: generic forced-induction V1 implementation transport

Failure signature: `patch/application integrity | encoded generic V1 payload decode/decompress | source-delivery layer`

- Attempt 1 — split gzip/base64 payload parts. Failed during application/transport.
- Attempt 2 — replacement/consolidated encoded payload transport. Failed during application/transport.
- Attempt 3 — gzip-trailer bypass. Failed with `generic forced-induction payload is not gzip data`.

Decision: **strategy retired**. No further encoded/compressed generic implementation payload is permitted.

Replacement strategy: use readable repository patch/source files with explicit anchors and post-condition checks before compilation.

## Gate 2 incident: PowerShell transcript capture

Failure signature: `configure | CMake developer warning promoted to NativeCommandError | CI logging/harness layer`

- Attempt 1 — Windows workflow run 50 reached enhanced configure after all source preconditions and topology post-conditions passed. CMake emitted a developer warning on stderr while its output was piped through `Tee-Object`; global `ErrorActionPreference = Stop` converted that stderr record into a terminating `NativeCommandError` before CMake's exit status could be evaluated. The generated source did not reach compilation. The repair keeps transcript capture but temporarily uses `ErrorActionPreference = Continue` only around the two native CMake commands, restores the original setting immediately afterward, and checks each native exit code explicitly. Result: run 50 failed in the harness layer; one evidence-based repair is allowed.

## Gate 4 incident: throttled-SI causal assertion

Failure signature: `runtime smoke | unjustified 10x open/closed delivery ratio | test assertion layer`

- Attempt 1 — Windows workflow run 53 compiled and linked the unchanged production core and passed four of five generic runtime smokes. The throttled-SI fixture delivered `0.00215828` kg through the open path and `0.00182856` kg through the closed path: the required causal ordering was present, but the test additionally demanded a 10x ratio that is not an architecture invariant and conflicts with Gate 4's no-performance-target scope. The repair removes only the magnitude multiplier and retains the directional assertion `openDelivery > closedDelivery`. Production code, geometry and calibration remain unchanged. Strategy remains allowed for one evidence-based test-layer repair.

## Gate 6 incident: loaded-transient source post-condition

Failure signature: `patch/application integrity | missing Alco251B token in loaded-transient source | verification layer`

- Attempt 1 — Exact-source reconstruction copied every readable file, then rejected the new test because the application post-condition searched the C++ source for the CTest suite name `Alco251B`; that name correctly exists in CMake, while the source identifies the invariant as `requireNative251B`. The repair changes only the mismatched verification token. A fresh upstream reconstruction then passed all application preconditions and post-conditions. Strategy remains allowed; no production or calibration value changed.

## Gate 6 diagnostic incident: legacy static-audit scope

Failure signature: `static audit | missing seed-owned files | audit invocation layer`

- Attempt 1 — Running the legacy overlay audit against the generated upstream source reported overlay-only files as missing.
- Attempt 2 — Running it against the pre-seed repository reported seed-owned generated files as missing. The evidence shows this audit is only valid after CI seed restoration and is not a local Gate 6 source audit. No further use is allowed in this layer; Gate 6 instead relies on the dedicated readable-application pre/post-conditions, failure-loop guard, diff checks, and scoped Windows build/test.

## Gate 6 incident: 16-251B script compilation

Failure signature: `runtime setup | 16-251B reference script did not compile | MR configuration layer`

- Attempt 1 — Windows run 56 configured and built the scoped executable, discovered exactly two tests, and both stopped during compilation of `assets/alco_16_251b_main.mr`. The Piranha compiler wrote the detailed parse/type error only to a working-directory `error_log.log`, which was not in the CI artifact. The next commit changes only the Gate 6 harness to include that existing compiler log in the failing test output. This is diagnostic evidence collection, not an MR, physics, topology or calibration repair.
- Attempt 2 — Windows run 57 surfaced the exact compiler error: `alco_16_251b_native(602): error R0030: Port not found` for `backflow_atmospheric_mixing`. The reference file came from the official v0.1.14a distribution, where that exhaust-system port exists; the reconstructed donor library did not expose it. Commit `2656611bc9c584404fa971c615f96198dc7fb952` added the missing readable port and physical reverse-flow atmospheric mixing without changing turbo topology or calibration. Result: subsequent runs compiled the script. This MR-configuration strategy is exhausted; no third repair is permitted in this layer.

## Gate 6 incident: script execution produced no engine

Failure signature: `runtime setup | compiled script executes without producing Engine | interpreter execution layer`

- Attempt 1 — Windows run 58 reconstructed, configured and built successfully, and the 16-251B script compiled. Both scoped tests then stopped at `16-251B reference script produced no engine`. The next change exposed Piranha's runtime execution error text; it did not alter scripts, physics, routing or calibration.
- Attempt 2 — Windows run 59 reproduced the same null-engine result. The newly surfaced runtime error was empty: compilation and execution returned, but the output engine channel was never populated. This rules out another hidden exception-message variant. The interpreter-diagnostic strategy is exhausted; no third diagnostic patch is permitted in this layer.

Reclassification: `public script-library/native-donor mismatch | obsolete engine action type boundary | v0.1.14a compatibility layer`.

New evidence: a normalized comparison against the exact official v0.1.14a distribution shows that the reconstructed donor's `es/actions/actions.mr` consumes `[engine]` at `set_engine`, `_add_crankshaft` and `_add_ignition_module`, while both the reconstructed object library and official v0.1.14a expose `[engine_channel]`. The next strategy is a pinned, readable three-boundary compatibility patch. It is gated first by an unmodified official v0.1.14a naturally aspirated SI script, then by the existing 16-251B tests. Unsupported v0.1.14a Wankel and other native APIs are intentionally not copied into the older native donor.

- Compatibility attempt 1 — Windows run 60 verified the exact release archive and unmodified Kohler CH750 hash, built successfully, and passed `V014aCompatibility.StockSiLoads`: two-cylinder SI, forced induction disabled, original exhaust routing. The subsequent ALCO tests reported the old line-602 port error only because the connector-published commit was assembled on a stale local tree and omitted the already-accepted backflow files. That result does not consume another MR-configuration attempt.

## Gate 6 incident: connector commit tree omitted accepted parent files

Failure signature: `publication | connector tree based on stale local equivalent commit | commit-construction layer`

- Attempt 1 — Commit `15106c8628e1a574ae6b12a999f0c91e85cea1ce` used remote parent `b3ccaf7...` but supplied the base tree SHA from stale local commit `d1312d9`. Run 60 therefore lost the accepted backflow changes and reproduced line 602 `R0030` after the stock SI gate passed. The repair reconstructs the commit from the actual remote `b3ccaf7` tree and merges the six intended compatibility files. No model, physics, topology or calibration change is allowed in this repair.

Result: corrective commit `d9de10db7176ade22d4129a898dd4f0b00ee4ec2` restored the exact remote-parent tree. Run 61 compiled the ALCO script without the line-602 error, confirming the publication repair.

## Gate 6 incident: imported 16-251B module produced no root action

Failure signature: `runtime setup | compiled entry script produced no engine and no runtime error | MR entrypoint layer`

- Attempt 1 — In corrected run 61 the exact official v0.1.14a Kohler null passed, and the 16-251B script compiled, but both ALCO cases returned no engine with an empty runtime-error string. The root `assets/alco_16_251b_main.mr` contained only an import, while the `main()` invocation was inside the imported definition module. Existing Engine Simulator entry scripts invoke `main()` in the root compilation unit. The repair moves only that invocation to the root entry file and adds application postconditions; the engine definition, calibration, gas path and C++ physics remain unchanged.

Result — Windows run 62 compiled and executed the 16-251B script, produced an `Engine`, created a simulator and reached both loaded-transient assertions. The entrypoint incident is closed.

## Gate 6 incident: 16-251B burned-fuel response

Failure signature: `loaded transient | higher rack and direct injection do not increase burned fuel | combustion layer`

- Attempt 1 / evidence only — Windows run 62 passed the exact official v0.1.14a naturally aspirated SI null, then loaded the 16-251B and entered its causal transient. The ordered assertions prove that the high notch command increased fuel rack and direct injected mass. The next assertion failed exactly: `GATE6_FAIL classification=combustion mode=alco-251b-causal reason=higher direct injection did not increase burned fuel`. Later exhaust, turbine, shaft, compressor and charge assertions were not evaluated, so this run does not prove or disprove dynamic 16-251B turbo causality. No repair, calibration change or retry was made.

Status: **implementation stopped by user instruction after this failure**. Before any future repair, collect combustion-state evidence that distinguishes no combustion from a missing incremental response. Do not change starter torque, compression ratio, injection/burn calibration, turbo calibration or assertion thresholds to hide the result.

## Gate 6 incident: 16-251B rack release

Failure signature: `loaded transient release | rack does not retreat within five-second release window | control layer`

- Attempt 1 / evidence only — The independent Windows run 62 release case loaded and ran the same 16-251B sequence. Its first release assertion failed exactly: `GATE6_FAIL classification=control mode=alco-251b-release reason=fuel rack did not retreat after notch release`. Injection, turbine-power and shaft-release assertions were not evaluated after that failure. No repair, gain change, timeout change or retry was made.

Status: **implementation stopped by user instruction after this failure**. A future diagnosis must capture target speed, held speed, rack, governor error and controller state through command release before deciding whether the cause is wind-up, command mapping, persistent state or a fixture assumption. Keep this investigation separate from combustion and turbo physics.

Run 62 evidence: workflow `36584792845`; artifact `engine-sim-gate6-alco-251b-loaded-transient` (ID `11041298653`, SHA-256 `664eedb81b9b94253a0bf4cb5efc84b167996107d609e6e4e73266ba1707c389`). The official v0.1.14a stock SI null passed in 1.09 seconds; the two 16-251B cases failed in 21.90 and 30.15 seconds respectively.

## Gate 6B diagnostic: 16-251B governor observability

Failure signature (harness only): `diagnostic | 16-251B governor evidence incomplete | test-harness layer`

Purpose: collect the control-layer evidence missing from Run 62 before any repair. Analysis in `docs/GATE6_RUN62_DIAGNOSIS.md`.

- Attempt 1 / evidence only — authorized by the user after the Run 62 diagnosis. Adds `test/alco_251b_governor_observability.cpp`, target `engine-sim-governor-observability`, CTest `Alco251BObservability.Governor` and CI mode `ALCO_251B_GOVERNOR_OBSERVABILITY_ONLY`. The Run 62 loaded sequence is reproduced exactly, followed by one command-0.0 probe. The harness records commanded/applied control, target and held speed, signed crank speed, governor error, output and internal rack, and a shadow governor model driven by the observed speed. It reports one classification (`FIXTURE_INVALID`, `COMMAND_MAPPING`, `RUN62_NOT_REPRODUCED`, `FIXTURE_ASSUMPTION`, `PERSISTENT_STATE`, `RELEASE_LOGIC_OR_WINDUP`, `UNCLASSIFIED`) and passes on evidence completeness, not on a physics outcome. Production C++, MR and calibration are unchanged; `tools/apply_forced_induction_v1.py` now pins every production template to its `71fff99` content. The Run 62 loaded-transient assertions are not run. Static prediction before the run: `FIXTURE_ASSUMPTION`.

Result — Windows run 63 (`36607531113`, commit `62ffad78b12f4e5ac42a4600f55430ef08270d48`) built the real target, passed the official v0.1.14a SI null, and completed the evidence in 38.8 s. Artifact `engine-sim-gate6b-alco-251b-governor-observability` (ID `11052855913`, SHA-256 `536489f56e4797fb8f29f1818e039e46ac9b315f89f17fde1ee6f0be59a477ce`). Classification: `FIXTURE_INVALID`, triggered solely by rotation direction: signed crank speed was `+62.831853 rad/s` in every measured phase, equal to the dyno setting `+units::rpm(600)`, while the simulator's starter drives `-26.18 rad/s` and `Engine::isSpinningCw()` treats `v_theta <= 0` as normal rotation. The Run 62 fixture therefore holds the 16-251B at 600 rpm in reverse. Held speed magnitude was exactly 600 rpm throughout.

All governor evidence below the direction check matches the `FIXTURE_ASSUMPTION` prediction, and it is valid despite the direction because the governor consumes `|speed|`:

- applied command equals commanded in every phase; shadow-model divergence is exactly 0, so the MR governor constants and update law are confirmed;
- high: error 7018.39 rad²/s², rack 0.626 → 1.0, 4.93 s of 5.51 s at the upper limit, mean 0.980;
- release: error exactly 0 throughout, rack pinned at 1.0 for the whole window, so `mean(release) = 1.0 >= mean(high) = 0.980` reproduces Run 62;
- release internal rate decays from 0.461/s to 0.0030/s at 5τ: no persistent state or wind-up;
- probe (command 0.0, target 400 rpm): error −2193.25, rack falls 1.0 → 0.086 at the predicted −0.146/s: release logic works.

Control-layer conclusion: the Run 62 release failure is a fixture/command-mapping assumption (release target equals the held speed), not governor logic, wind-up or persistent state. Separate fixture finding: the shared Run 62 dyno fixture spins the engine in reverse; this also invalidates the Run 62 combustion observation until re-measured. Measured windows ran 2.205/5.5125 s for 120/300 frames (147 steps per 1/60 s frame), about 10 % longer than nominal; this does not change any conclusion. No repair was made. Strategy remains allowed; evidence is complete.

This entry does not consume a control-layer repair attempt. A classification other than evidence-incomplete is the result, not a failure; any repair it suggests requires separate authorization.

## Gate 6 fixture correction: reverse dyno hold and held-speed release

Failure signature: `loaded transient | dyno holds engine in reverse and release target equals held speed | test-fixture layer`

- Attempt 1 — authorized by the user after Run 63. Evidence: Run 63 measured signed crank speed `+62.83 rad/s` against the simulator's forward (starter) direction `-26.18 rad/s`, and release error exactly 0 with the rack pinned at 1.0. The change is confined to the shared fixture in `test/alco_251b_loaded_transient_validation.cpp` and `test/alco_251b_governor_observability.cpp`: the dyno holds 600 rpm with the starter's sign (`std::copysign`), and release commands idle (0.0, target 400 rpm) instead of a target equal to the held speed. Gate 6B's probe phase is removed because release now performs it, and its classifications become `FIXTURE_INVALID`, `COMMAND_MAPPING`, `RELEASE_RESPONDS`, `FIXTURE_ASSUMPTION`, `PERSISTENT_STATE`, `RELEASE_LOGIC_OR_WINDUP`, `UNCLASSIFIED`. No assertion, threshold, window length, gain, calibration, MR or production file changed; production templates remain pinned to `71fff99`, and the application tool now forbids the reversed hold and the held-speed release. Confirmation run: Gate 6B only. Expected: `RELEASE_RESPONDS`. The Gate 6 loaded-transient tests and Gate 6A are not run.

Result — Windows run 64 (`36610061073`, commit `15d17cad243a6c49569cb0d64a8a3db0800409b6`) built, passed the v0.1.14a SI null, and completed the evidence in 43.6 s. Artifact `engine-sim-gate6b-alco-251b-governor-observability` (ID `11053462759`, SHA-256 `e76bcba45ee5e94ee5452a95fa8e5c7950305a5dea9933b55df0fd1c513f2c1b`). Classification: `FIXTURE_INVALID`. **The correction failed.** With `m_dyno.m_rotationSpeed = -62.83 rad/s` the dyno did not hold the engine: held speed was 0.092 rpm mean (max 0.18 rpm) in baseline, high and release, signed speed about −0.001 rad/s. The engine was stationary after the starter phase ended. Consequently the governor saw error `target²` (3947.8 / 10966.2 / 1754.6), the rack stayed at 1.0 in every phase, and the release assertion was not satisfied. Command mapping and the shadow model were again exact (divergence 0).

Evidence gained:

- Run 63 (`+62.83`) drove the crank to exactly `+62.83 rad/s`; Run 64 (`-62.83`) left it at ≈ 0. The dynamometer therefore does not act symmetrically. The observed behaviour fits a one-sided (absorbing) torque limit: it can drive the crank toward a positive target but cannot motor it in the negative (starter) direction. This is a hypothesis; the dynamometer source is in the pinned upstream donor and was not inspected.
- After 3 s of forward cranking by the starter (−250 rpm target) with the rack at 1.0, the engine did not keep turning once the starter was released. That is new combustion-relevant evidence (no self-sustained running), but the start/settle trace that would show the cranking speed and decay is in the artifact CSV, which the diagnosing environment could not download.

Hypothesis for any next step: no dyno setting in the current fixture can hold the 16-251B at 600 rpm in its forward direction. The two loaded tests cannot be made valid by changing only the dyno sign. Test-fixture attempt 1 of 2 is consumed. Implementation is stopped under the standing instruction that any further failure stops work; no second fixture change was made.

Missing evidence before another attempt: the upstream `Dynamometer` constraint (sign convention and torque limits), and the Run 64 start/settle trace (cranking speed reached, decay after release).

## User-requested review build

Not a validation gate. After Run 64 the user explicitly asked for a program to run and a source tree to review. `REVIEW_BUILD_ONLY=1` snapshots the reconstructed, fully patched source (without `.git`, with `BUILDING.txt`), then builds `engine-sim-app` and the diagnostic tools, packages them with the seed's `tools/package_runtime.ps1`, and adds 16-251B launch scripts and `STATUS.txt`. No tests, regression comparison or loaded checks run, and production templates stay pinned to `71fff99`. This is the first GUI build and packaging since V1; if it fails, the failure is recorded here as a packaging-layer signature and is not an engine-physics failure.

## Handover incident: transient worktree loss

Failure signature: `handover | uncommitted temporary worktree unavailable on continuation | workspace persistence layer`

- Attempt 1 — The first `CLAUDE.md` draft existed only as an untracked file in a temporary worktree. On the next turn that worktree was no longer present. The remote implementation commit and all CI artifacts were intact. No implementation was repeated or changed. The handover was reconstructed in a fresh worktree checked out directly from remote commit `71fff99ee8b78666107f006028c223933ab41f55`, and is being committed immediately as documentation-only work.

Decision: do not leave future handover or failure evidence uncommitted across turns. This incident is operational, not an Engine Simulator implementation failure.

## Logging rule

For each future repeated failure, record:
- signature;
- attempt number;
- evidence gained;
- hypothesis;
- change made;
- result;
- whether the strategy remains allowed.

At attempt 2 in the same layer, a third repair may not use the same strategy. At three distinct failed strategies, implementation stops for a root-cause report.
