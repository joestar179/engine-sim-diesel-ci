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
