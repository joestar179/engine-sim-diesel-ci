# Claude Code Handover — Engine Simulator Diesel / Forced Induction

Last updated: 2026-09-29

Repository: joestar179/engine-sim-diesel-ci

Working branch: turbo-architecture-v1

Final implementation commit before handover:
71fff99ee8b78666107f006028c223933ab41f55

This repository is an enhancement overlay and deterministic CI reconstruction
project, not a complete Engine Simulator source checkout. Read this file before
running or changing anything.

## 1. Mandatory reading and stop state

Read these files completely and in this order:

1. docs/TURBO_ARCHITECTURE_V1.md — locked architecture contract.
2. docs/FAILURE_LOOP_GUARD.md — mandatory development procedure.
3. docs/FAILURE_ATTEMPT_LOG.md — attempt budgets and current failures.
4. docs/GENERIC_FORCED_INDUCTION_V1_IMPLEMENTATION.md — implementation map.
5. docs/V014A_COMPATIBILITY_BASELINE.md — baseline provenance.

Implementation is stopped. Run 62 exposed new combustion and control failures,
and the user explicitly instructed that any further failure must stop work and
be captured in this handover. Do not retry, tune or patch until the user
authorizes a new evidence-based diagnostic gate.

Never:

- invent a replacement turbo architecture;
- impose boost or intake pressure as a target boundary;
- use turbo output as a fuel or power multiplier;
- put ALCO-specific branches in production C++;
- use ExhaustSystem::m_system as a pre-turbine state;
- tune starter, fuel, governor, boost or test thresholds to hide a failure;
- combine combustion, control and turbo changes;
- reintroduce generic_forced_induction_v1_patch.py,
  generic_v1.part*.b64 or consolidated.part*.b64;
- start full validation, GUI packaging, Windows packaging or D3DX work without
  explicit authorization.

The deterministic .ci/seed.part*.b64 bootstrap remains allowed.
.ci/turbo_arch_patch.py and .ci/full_turbo_topology_patch.py are grandfathered
only for reconstruction of the accepted pre-V1 baseline. Do not extend them.

## 2. Current result

Generic Forced-Induction V1 is implemented in readable files. Gates 1 through
5 passed. Gate 6 did not pass.

Run 62:

- workflow ID: 36584792845
- URL:
  https://github.com/joestar179/engine-sim-diesel-ci/actions/runs/36584792845
- tested commit: 71fff99ee8b78666107f006028c223933ab41f55
- evidence artifact: engine-sim-gate6-alco-251b-loaded-transient
- artifact ID: 11041298653
- artifact SHA-256:
  664eedb81b9b94253a0bf4cb5efc84b167996107d609e6e4e73266ba1707c389

What passed in Run 62:

- exact official v0.1.14a archive and Kohler script hashes;
- official Kohler CH750 naturally aspirated SI null load;
- forced induction disabled on the SI null;
- original turbo-disabled exhaust routing;
- 16-251B script compilation and execution;
- creation of Engine and Simulator objects;
- initial V16, compression-ignition, common-intake, four-exhaust-branch,
  TurboGroup, four-scroll and post-turbine routing checks;
- finite/physical state checks during the measured windows until the causal
  assertion stopped each test.

Exact failures:

1. Combustion:
   GATE6_FAIL classification=combustion mode=alco-251b-causal
   reason=higher direct injection did not increase burned fuel
2. Control:
   GATE6_FAIL classification=control mode=alco-251b-release
   reason=fuel rack did not retreat after notch release

The first test had already proved higher command -> higher rack -> more direct
injection. It stopped at burned fuel, so later exhaust/turbine/shaft/compressor/
charge assertions were not evaluated. The second test stopped at the first
release assertion, so later release checks were not evaluated.

This means the earlier root-entry/no-engine problem is closed. It does not mean
dynamic 16-251B turbo causality is validated.

## 3. Source baseline — describe it accurately

Native source donor:

- repository: ange-yaghi/engine-sim
- commit: 56725cc012581282567900b15871018d55b7ab42
- describe: v0.1.11a-6-g56725cc
- Piranha: 432f0b122bb1663b686c553c7e7269300afac3bc
- Delta Studio: b7d0a046733b924d12706baf1e5e59ba427aa7b1

This is a known, pinned native baseline. It is not v0.1.14a native source.
Community Edition v0.1.14a is distribution-only; its tag and release archive do
not contain the application C++ source.

Official compatibility reference:

- archive URL:
  https://github.com/Engine-Simulator/engine-sim-community-edition/releases/download/v0.1.14a/engine-sim-v0.1.14a.zip
- archive SHA-256:
  2fc1e7c2ad6a94af4bbe78e69907b57aad3d5ebda7d55563e24fa2e14707fbe6
- null script: assets/engines/kohler/kohler_ch750.mr
- null SHA-256:
  c729091fe9c6d3ad3761564850bfad437ea441fee7308a7e7c7ea226ad8b2196

CI verifies and temporarily copies only that stock SI null. It does not commit
the release asset.

A normalized v0.1.14a script-library comparison found material differences in
actions, units, theme, objects, heads and application settings. Only supported,
evidence-backed compatibility was ported. Do not copy v0.1.14a Wankel actions,
k_32inH2O or other interfaces lacking native support.

The readable transform in tools/apply_forced_induction_v1.py changes the three
engine action boundaries set_engine, _add_crankshaft and
_add_ignition_module from engine to engine_channel. Its normalized old-actions
precondition hash is:
95510e0e7e9252a28c02a2f07da9e31e9ffcadd5e32666f5e1170c0932e33d49

## 4. Reconstruction order

The workflow reconstructs the buildable tree in this order:

1. Checkout this overlay branch.
2. Run .ci/guard_failure_loops.py.
3. Join and verify the deterministic seed archive.
4. Restore seed-owned tools and overlay files.
5. Run .ci/diagnostic_patch.py.
6. Run tools/enable_gate2_compile.py tools/windows_ci.ps1.
7. Clone and pin the native donor and submodules.
8. Apply the accepted diesel/CI enhancement.
9. Apply the grandfathered accepted pre-V1 full-topology patch.
10. Apply .ci/restore_engine_metrics_patch.py.
11. Apply readable Generic Forced-Induction V1 through
    tools/apply_forced_induction_v1.py.
12. Configure, build and run only the selected gate.

The readable V1 application pins normalized hashes for replaced baseline files,
rejects unexpected pre-existing V1 files, installs templates from
patches/forced_induction_v1, performs anchored compatibility transforms and
checks required/forbidden topology tokens. Do not weaken those checks.

## 5. Implemented topology

Air, per TurboGroup:

ambient reservoir -> inlet/filter restriction -> compressor ->
compressor-discharge GasSystem -> optional charge-air cooler GasSystem ->
optional throttle/air valve -> charge-plenum GasSystem ->
existing Intake::m_system -> runner/intake valve -> cylinder

The compressor transfers bounded real gas mass. Source moles decrease and
destination moles increase. Compressor work raises delivered-gas energy and is
debited from the shaft. Charge pressure emerges from mass, energy, volume,
restrictions and cylinder demand. The V1 path does not set an arbitrary boost
boundary. At zero shaft speed active compressor head/work are zero; the passive
path remains possible for cranking.

Exhaust:

cylinder -> existing exhaust valve/port -> runner/primary -> configured pulse
group -> dedicated pre-turbine scroll GasSystem -> turbine swallowing
restriction -> energy extraction from the same transferred gas ->
original ExhaustSystem::m_system -> original outlet restriction -> atmosphere

The original ExhaustSystem::m_system is post-turbine. Turbine flow uses real
GasSystem transfer. Pressure ratio and extraction come from that same flow.
Extracted energy is bounded by requested work, transferred-gas energy and
available post-transfer gas energy.

When forced induction is disabled, Engine::getExhaustDestination returns the
original ExhaustSystem::m_system and no extra charge or scroll volumes are
connected.

Shaft:

turbine torque - compressor torque - bearing/friction torque =
inertia times angular acceleration

Each group owns its rotating state. Starting turbine torque can exist at zero
shaft speed; a stationary compressor cannot create pressure rise.

## 6. Generic and multi-group ownership

ForcedInductionSystem::Parameters owns a vector of group parameters and
ForcedInductionSystem owns a vector of TurboGroup. Exhaust routes identify a
group and scroll; duplicate ownership is rejected. Multiple groups can feed an
explicitly shared intake. Gate 3 and Gate 4 cover separate two-group state,
parallel routing, shared intake and twin scroll.

Production C++ is combustion-agnostic. It contains no ALCO, diesel, rack or
governor-specific routing.

The backward-compatible MR adapter currently instantiates one group. Native
multi-group MR authoring remains missing, but adding that surface should not
require a gas-path rewrite.

## 7. Optional devices

Physically implemented and inert when disabled:

- charge-air cooler acting on actual stored/flowing gas;
- cooler pressure-loss restriction;
- charge-path throttle/air valve;
- wastegate moving real gas from scroll to post-turbine exhaust;
- compressor bypass recirculating real gas to inlet or venting to ambient;
- VGT/VNT actuator changing turbine swallowing capacity;
- actuator state/response timing;
- telemetry for scroll, turbine, shaft, compressor, cooler, plenum,
  wastegate, bypass and intake delivery.

Architecturally exposed but not supplied as finished policies:

- closed-loop boost/wastegate control;
- closed-loop VGT control;
- ECU/pneumatic actuator policies;
- smoke/air limiter consuming physical air state;
- sequential-turbo routing and control;
- EGR and exhaust-brake branches;
- native multi-group MR authoring;
- measured compressor/turbine maps and a full surge model.

The compressor/turbine model is a bounded reduced-order approximation.
TurbochargerModel::step remains for legacy compatibility, but Generic V1 uses
compressorOperatingPoint, bounded transfer, processTurbineFlow and
advanceShaft. Never use the legacy output pressure as an intake boundary.

## 8. Combustion and control boundary

Forced induction does not own fueling, ignition, governor or rack logic.
Compression ignition/direct injection, the diesel governor/fuel rack and
procedural diesel audio are separate accepted subsystems. Turbo exposes physical
air and exhaust state to them.

The current Run 62 failures are specifically outside the turbo ownership
boundary until evidence proves otherwise:

- combustion did not show increased burned fuel after increased injection;
- governor/control did not retreat the rack after command release.

Do not alter turbo topology to repair those observations.

## 9. Gate history

- Gate 1: deterministic reconstruction/source inspection complete.
- Gate 2: scoped compile/link passed in run 51 at 444bea466b0aeb716712ba67184118167cac3c13.
- Gate 3: seven architecture-invariant groups passed in run 52 at
  21b0bf3520a0aa317127d252098e3fa78df93e04.
- Gate 4: five synthetic runtime smokes passed in run 54 at
  952b75ac1d5ca25e4dac7076fdabf77d0a5d095a after removing only an
  unjustified 10x throttle ratio assertion.
- Gate 5: seven isolated integration cases passed in run 55 at
  ab0284f8c18176b80c949681d0c09fd5bac6bd82.
- Gate 6: official stock SI null passed in run 62; two 16-251B loaded tests
  reached runtime and failed at combustion and control boundaries.

Gate 3 groups: disabled original path; compressor/passive flow/shaft power;
turbine route/restriction/power; transferred-gas energy bound; optional-device
inert/enabled behavior; two independent groups/shared intake; mass balance.

Gate 4 scenarios: naturally aspirated SI, turbo throttled SI, turbo unthrottled
air path, parallel two-group routing and twin-scroll routing. These test
causality/stability, not power.

Gate 5 checks passive cranking, exhaust-to-scroll, turbine-to-shaft,
compressor-to-charge, governor separation and actual-gas aftercooling. It uses
the older 6-251D only as a secondary case because it predates selection of the
16-251B reference.

The full suite has not been run after V1. The GUI/app has not been built.
No testable Windows simulator package has been produced. Compile success is not
project acceptance.

## 10. Gate 6 failure history

See docs/FAILURE_ATTEMPT_LOG.md for the authoritative attempt accounting.

- Run 56: script compile failed; compiler detail not captured.
- Run 57: exact missing backflow_atmospheric_mixing port exposed.
- 2656611 restored that v0.1.14a input as physical reverse-flow mixing.
- Run 58: script compiled but produced no Engine.
- Run 59: runtime-error channel was empty; no Engine.
- Interpreter-diagnostic layer exhausted after two attempts.
- Run 60: official SI passed, but connector publication used a stale base tree
  and omitted accepted backflow files; ALCO result invalidated.
- d9de10d restored the actual remote parent tree.
- Run 61: SI passed; ALCO compiled but produced no Engine.
- Static evidence showed main() was called only inside the imported module.
- 71fff99 moved only the invocation to the root script and added postconditions.
- Run 62 proved that entrypoint repair, then exposed the combustion and control
  failures recorded above.

Do not add another no-engine diagnostic. Do not rerun Run 62 unchanged.

## 11. ALCO 16-251B model and source confidence

Primary files:

- patches/forced_induction_v1/assets/alco_16_251b_main.mr
- patches/forced_induction_v1/assets/engines/alco/alco_16_251b_native.mr
- patches/forced_induction_v1/test/alco_251b_loaded_transient_validation.cpp

The supplied MI-1016B scan was checked directly. It confirms:

- 16 cylinders;
- 9 x 10.5 inch bore/stroke;
- 10,688 cubic inches;
- 13:1 compression ratio;
- the 16-cylinder firing order;
- turbosupercharging;
- 400-rpm idle and 2400 BHP at 1000 rpm;
- documented complete-engine, crankshaft/extension, piston-and-rod,
  turbosupercharger, exhaust-manifold and aftercooler weights;
- aftercooler placement after turbo compressor discharge;
- separate MI-1003 as the turbo-maintenance reference.

Do not overstate the scan. The current 45-degree bank, four pulse branches and
Model 710 identity are retained high-confidence reference-model configuration,
but were not independently located in the pages inspected for this handover.
The user's PDF was an external session attachment and was not committed; obtain
it again for page-level verification.

Current 16-251B forced-induction configuration:

- one fixed-geometry TurboGroup;
- four separate pre-turbine scroll states;
- one shaft and compressor;
- one common intake;
- real-gas aftercooler;
- wastegate, bypass and VGT disabled;
- four original ExhaustSystem objects used as pulse branches;
- exhaust_r_a selected as common post-turbine ExhaustSystem.

Still estimated or simulator calibration, not factory-exact:

- injection rate/duration and pump-delivery curve;
- ignition delay and burn fractions/duration;
- compressor/turbine maps;
- turbo inertia, volumes and flow capacities;
- runner lengths and flow coefficients;
- exact classic-16B rod length;
- detailed cam lift/angle card;
- governor gains;
- drivetrain/electrical-load surrogate.

The model identifies MI-1000 as needed injection/overhaul data and MI-1003 or
measured maps as needed turbo data.

The configured 250-rpm starter and 15,000 lb-ft starter torque are explicitly a
simulator accommodation, not an ALCO specification. Do not raise or retune
them. Diagnose combustion causally before deciding whether they can be removed.

## 12. Safe CI usage

The readable adapter supports these mutually scoped modes:

- CORE_COMPILE_ONLY=1
- ARCHITECTURE_TESTS_ONLY=1
- GENERIC_RUNTIME_SMOKE_ONLY=1
- ALCO_INTEGRATION_ONLY=1
- ALCO_251B_LOADED_TRANSIENT_ONLY=1
- ALCO_251B_GOVERNOR_OBSERVABILITY_ONLY=1 (Gate 6B, diagnostic only)
- REVIEW_BUILD_ONLY=1 (user-requested review build: source snapshot, GUI app,
  diagnostic tools and runtime package; runs no tests and validates nothing)

Each mode configures/builds scoped targets, verifies exact test discovery,
writes evidence and exits before full regression, GUI and packaging.

The current workflow selects only ALCO_251B_GOVERNOR_OBSERVABILITY_ONLY=1
(Gate 6B, authorized after the Run 62 diagnosis in
docs/GATE6_RUN62_DIAGNOSIS.md). Do not reselect
ALCO_251B_LOADED_TRANSIENT_ONLY=1 unchanged. Gate 6A (combustion
observability) is proposed but not authorized.

Gate 6B Run 63 result: evidence complete, classification FIXTURE_INVALID.
The Run 62 dyno fixture holds the engine at +600 rpm, which is reverse
rotation in this simulator. The control failure is explained by the fixture
(release target equals held speed); the governor itself behaves correctly.
Do not run Gate 6A or any loaded test on the reversed fixture.

The user authorized a test-layer fixture correction (dyno held with the
starter's sign, release to idle, in both loaded tests). Gate 6B Run 64
showed it FAILED: with a negative dyno speed the engine sat at ~0.09 rpm, so
the dyno does not motor the engine forward, and the engine did not keep
running after 3 s of forward cranking at full rack. Test-fixture attempt 1 of
2 is consumed. Work is STOPPED pending user direction. Before any further
fixture change, obtain the upstream Dynamometer constraint (sign and torque
limits) and the Run 64 start/settle trace (artifact 11053462759). Gate 6A and
the Gate 6 loaded-transient tests remain unauthorized. Details in
docs/FAILURE_ATTEMPT_LOG.md.

After Run 64 the user explicitly requested a runnable program and a
reviewable source tree. The workflow therefore selects REVIEW_BUILD_ONLY=1,
which is the first GUI (engine-sim-app) build and runtime packaging since V1.
It is not validation: a successful package does not change the Gate 6 state.
Review build run 36612183389: source snapshot and all targets including
engine-sim-app built; packaging failed on the missing d3dx10_43.dll (legacy
DirectX runtime absent on the runner). The user authorized the runner-side
DirectX End-User Runtime install; the review build is being re-run.

The Windows environment used Visual Studio 2022 x64 RelWithDebInfo, CMake
3.31.12, Boost 1.78, SDL2/SDL2_image through vcpkg, winflexbison3, Piranha
enabled, Discord disabled and DTV disabled.

## 13. File map

- patches/forced_induction_v1/include/forced_induction_system.h — ownership,
  routes, device parameters and telemetry.
- patches/forced_induction_v1/src/forced_induction_system.cpp — real gas paths,
  cooling, valves, turbine extraction and shaft update.
- patches/forced_induction_v1/include/turbocharger_model.h and corresponding
  source — reduced-order rotating/map model.
- patches/forced_induction_v1/include/engine.h and src/engine.cpp — engine
  ownership/routing bridge.
- patches/forced_induction_v1/src/combustion_chamber.cpp — exhaust destination
  routing and separate CI integration.
- patches/forced_induction_v1/src/piston_engine_simulator.cpp — per-fluid-step
  forced-induction processing.
- patches/forced_induction_v1/es/objects/objects.mr — compatible single-group
  script surface.
- patches/forced_induction_v1/test/forced_induction_invariant_tests.cpp — Gate 3.
- patches/forced_induction_v1/test/forced_induction_runtime_smoke_tests.cpp —
  Gate 4.
- patches/forced_induction_v1/test/alco_integration_validation.cpp — Gate 5.
- patches/forced_induction_v1/test/alco_251b_loaded_transient_validation.cpp —
  Gate 6.
- tools/apply_forced_induction_v1.py — readable application contract.
- tools/enable_gate2_compile.py — scoped CI adapter.
- .github/workflows/windows-ci.yml — Windows workflow.

The compatibility and 16-251B integration work from b3ccaf7 through 71fff99
changed or added exactly these repository files:

- docs/FAILURE_ATTEMPT_LOG.md
- docs/V014A_COMPATIBILITY_BASELINE.md
- patches/forced_induction_v1/CMakeLists.txt
- patches/forced_induction_v1/test/alco_251b_loaded_transient_validation.cpp
- tools/apply_forced_induction_v1.py
- tools/enable_gate2_compile.py
- patches/forced_induction_v1/assets/alco_16_251b_main.mr
- patches/forced_induction_v1/assets/engines/alco/alco_16_251b_native.mr

No production turbo C++ was changed in that final compatibility sequence. The
generated es/actions/actions.mr compatibility change is applied during
reconstruction by the readable tool.

## 14. Authorized next step

Ask the user before reopening implementation. If diagnosis is authorized, keep
the two failures separate and collect evidence before changing behavior:

1. Combustion investigation: record low/high injected mass, burned mass,
   cylinder pressure, temperature, ignition eligibility and burn-state
   counters. Determine whether there is no combustion or no incremental burn
   response. Do not change injection, autoignition, burn duration, compression
   ratio, turbo or starter values during evidence collection.
2. Control investigation: record target speed, held speed, rack, governor error
   and controller/integrator state across baseline, high command and release.
   Determine whether the result is wind-up, command mapping, persistent state
   or an invalid fixture assumption. Do not retune gains, extend the window or
   weaken the assertion first.

Only after one layer has a causal diagnosis should one small repair and one
scoped test be proposed. Turbo calibration, power tuning, full-suite
validation, packaging and D3DX remain outside scope.

## 15. Publication and workspace caution

Commit 15106c8 used the correct remote parent but a stale local base tree,
silently omitting accepted files. If constructing trees through an API, always
resolve the actual remote HEAD tree and preserve file modes.

The first handover draft was an untracked file in a temporary worktree that was
not available on continuation. The implementation and CI artifacts were not
lost. This handover was reconstructed from remote commit 71fff99 and the Run 62
artifact, then committed immediately. Do not leave future failure evidence
uncommitted across turns.
