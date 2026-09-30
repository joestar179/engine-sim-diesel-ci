# Claude Code Handover — Engine Simulator Diesel / Forced Induction

Last updated: 2026-09-29

Repository: joestar179/engine-sim-diesel-ci

Working branch: turbo-architecture-v1

Final implementation commit before handover:
71fff99ee8b78666107f006028c223933ab41f55

This repository is an enhancement overlay and deterministic CI reconstruction
project, not a complete Engine Simulator source checkout. Read this file before
running or changing anything.

## 0a. Standing directive — token economy

KEEP TASKS FOCUSED ON THE TOPIC AT HAND. Limit additional scope, extra
scripts, exploratory runs and verbose output to what the current request
needs. Prefer the smallest set of measurements that answers the question,
reuse existing tools (probe, bench, audio-render) instead of writing new
ones, and propose follow-up work rather than doing it unasked.

## 0. Standing requirement — engine sound

Engine Simulator is a sound generator as much as a physics simulation. Every
change (physics, MR configuration, calibration, turbo, combustion, control,
logging or tooling) must be assessed for its effect on the produced sound
before it is considered done:

- state which audio inputs the change touches: per-cylinder exhaust-runner
  pressure (the main exhaust-channel source in
  PistonEngineSimulator::writeToSynthesizer), combustion pressure-rise rate,
  turbo shaft speed and compressor power (procedural diesel audio), firing
  intervals, and per-frame CPU cost (which changes steps per frame and the
  synthesizer latency);
- compare the relevant source signal before and after where practical
  (for example runner-pressure pulses per bank from the cylinder probe);
- report audible consequences (firing evenness, missing or inverted pulses,
  surging idle, turbo whine) alongside the physics result;
- never accept a physics fix that silently degrades or breaks the audio path.

## 0b. Open issues and parameter register (keep current)

Status key: S = sourced, D = derived from documented data or measurement,
A = assumption. Replace every A with S or D before relying on it.

Open issues (highest impact first):

1. **16-251B efficiency/boost mismatch.** With the sourced fuel stop (0.84 g) the turbo beats no-turbo (2530 vs 2038 hp) but the sim SFC is 148 vs documented 168 g/BHP.h and boost 135 vs 258 kPa: the modelled engine is too efficient, so it needs less air and makes less exhaust energy. Source the fuel energy input, wall temperature and heat-release shape. 6-251D (350B) turbo data still unsourced.
2. **Ratings (checks, never targets):** 16-251B 2530 hp at 1000 rpm with the sourced fuel stop (rated
   2400); 6-251D 793 hp at 1100 rpm (class ~1200–1400). Fuel stops must come
   from documented BSFC or rack delivery, never be fitted to these ratings.
3. **Stock-engine impact is not assessed.** The enthalpy-transport fix in
   `GasSystem::flow` is physically correct but changes every engine's breathing,
   power and sound, including the stock spark-ignition engines.
4. **Sound:** low end weak with the current 16-251B MR settings (impulse
   response `minimal_muffling_01`, `hf_gain` 0.122, `noise` 1.0); A/B renders in
   `C:\es\run\renders\lf_ab`. Gas model uses gamma 1.40 for all gas (hot
   combustion gas ~1.28-1.33): likely cause of the too-high efficiency and low
   boost (proposed core fix).
5. **Gate 6B** shadow governor predates `k_p`, `droop` and the smoke limiter.
6. SI engines have no structure-borne knock layer (no pressure-rise rate).

Parameters:

| Parameter | Value | Status | Basis |
|---|---|---|---|
| Air O2 fraction (diesel/turbo paths) | 0.2095 | S | Composition of dry air; stock 0.25 kept for SI premixed intakes |
| Flow energy = enthalpy | (dof/2 + 1) R T per mol | S | First law for open systems |
| Hohenberg heat-transfer constants | 130, −0.06, 0.8, −0.4, +1.4 | S | Hohenberg, SAE 790825 |
| Smoke-limited equivalence ratio | 0.75 (combustion O2 limit; limiter λ = 1/0.75) | S (range) | Heywood, *ICE Fundamentals*: DI diesel ~0.7–0.8; midpoint chosen |
| Wall temperature in heat transfer | 90 °C | A | Stock value; real diesel surfaces ~400–500 K |
| Governor droop | 0.03 | A | Typical 3–5 %; the ALCO governor may be isochronous |
| Turbo friction law split | 0.3 const / 0.7 ∝ speed | A (fitted) | Keeps the documented 6-251D 90–180 s rundown |
| Turbo sound source | √(turbine + compressor power) | A | Modelling choice |
| Knock band / noise share | 1.6 kHz Q 1.5 / 0.3 | A | Qualitative (Austen & Priede); transient-dominant after user report |
| Turbo tone: tonal share / band Q / 2nd harmonic | 0.8 / 30 / 0.35 | A | Blade-pass tone dominant (user: "breeze, not whistle") |
| Knock / turbo global levels | 4e-4 / 15 | D | One reference render each |
| 16-251B fuel stop | 0.84 g | D | 720A rated SFC 168 g/BHP.h (IRIMEE) x 2400 BHP (MI-1016B) |
| Fuel energy input | 45.5 kJ/g x 0.88 = 40.0 kJ/g | A | Diesel LHV ~42.6-43 kJ/g; sim SFC 148 vs documented 168 g/BHP.h |
| 16-251B governor `k_p`/`k_s`/crank limit | 3 / 0.016 / 0.35 | D | Probe stability tuning |
| Simulation frequency | 3 kHz / 10 kHz | D | Measured real-time budget |

## 1. Mandatory reading and stop state

Read these files completely and in this order:

1. docs/TURBO_ARCHITECTURE_V1.md — locked architecture contract.
2. docs/FAILURE_LOOP_GUARD.md — mandatory development procedure.
3. docs/FAILURE_ATTEMPT_LOG.md — attempt budgets and current failures.
4. docs/GENERIC_FORCED_INDUCTION_V1_IMPLEMENTATION.md — implementation map.
5. docs/V014A_COMPATIBILITY_BASELINE.md — baseline provenance.

On 2026-09-30 the user explicitly authorized code changes and local
implementation resumed (see section 12, "Local repairs 2026-09-30"). Keep
recording every failure and change in docs/FAILURE_ATTEMPT_LOG.md, change one
layer at a time with evidence, and stop to report when a change exposes a new,
unexplained behaviour rather than stacking fixes.

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
DirectX End-User Runtime install. Run 36618461297 resolved D3DX, then
packaging stopped on the driver-provided vulkan-1.dll. Stopped; a single
packaging strategy change (report all unresolved DLLs at once, treat
driver-provided DLLs as documented prerequisites) awaits user authorization.
The user then built the review source locally; the engines run in the GUI.
Their local evidence (docs/FAILURE_ATTEMPT_LOG.md, "User local-run evidence")
corrects the Run 64 record: the held dyno locked the crank, and without it the
16-251B starts and runs. It shows the 16-251B burning 42 % of injected fuel
(6-251D: 88 %), a turbo shaft stalled near 200 rpm by the 25 N m friction
calibration, and an out-of-bounds audio array for 16 cylinders. No repair made.

Local per-cylinder probe (`engine-sim-cylinder-probe`, test-only, bit-identical
to the stock simulator): the R bank burns 98 % and the L bank 0 %. Every L
cylinder reaches TDC 45 deg before its R pin-mate, but the MR schedules the L
injection and cams 45 deg after it, so L injection lands at +66 deg ATDC. That
is an MR V-bank phasing error, not the combustion model. The low turbine-inlet
pressure has three causes together: idle load, only 8 cylinders firing, and a
turbine sized for full-load flow. The dyno holds |m_rotationSpeed| in the
current rotation direction, which explains Runs 63 and 64.

Local repairs 2026-09-30 (user-authorized; details in
docs/FAILURE_ATTEMPT_LOG.md):

1. MR V-bank phasing: bank_R at +V/2, bank_L at -V/2, crank tdc 90 deg + V/2,
   flip_display moved to the R head. Both banks now inject at -24 deg BTDC.
2. Compressor passive flow: a turning wheel keeps the pressure-driven passive
   path whenever discharge pressure is below inlet (the passive path previously
   closed once the shaft turned at all, starving the charge plenum to ~20 kPa).
3. Turbo lumped volumes (compressor inlet/discharge, cooler, plenum, scrolls)
   are stagnated every sub-step with GasSystem::dissipateVelocity(dt, 0). They
   previously kept undamped momentum near Mach 1 and rammed each other, so the
   turbo loop ran the engine on zero fuel.
4. Read-only TelemetryLog (GUI and probe): <exe dir>/../logs/telemetry_*.log,
   one SAMPLE line every 0.5 s of simulated time plus EVENT lines.

Result: all 16 cylinders fire, burning 99.8 % of injected fuel; 6-251D peak CI
temperature fell from 4111 K to 1887 K.

Second batch 2026-09-30:

- The governor gained `k_p` (proportional compensation) and
  `crank_rack_limit` (start-fuel limit held until idle is first reached). The
  16-251B MR uses k_p 3, k_s 0.016 and limit 0.35: no hunting, 1000 rpm held
  to +-0.3 rpm.
- Zero-step frames no longer turn the intake-flow gauges to NaN.
- The air gauges are display-smoothed.
- The 16-251B name is shortened (it overlapped the displacement text).
- The dead `lastValveLift[8]` array is removed.

Open:

- Turbo spool is slow under load (1.5 kg m^2 inertia, 25 N m friction).
- Turbo sound quality is poor (user-reported; next topic).
- The Gate 6B shadow governor predates `k_p`.

Third batch 2026-09-30 (root causes; details in docs/FAILURE_ATTEMPT_LOG.md):

- Flicker was the gauge needle integrator. Symplectic Euler with one step per
  frame went unstable below ~23 FPS. It is now sub-stepped at 1/120 s, and the
  earlier display smoothing is removed.
- Low FPS and stutter: this CPU (i7-7700HQ) could not run the engines in real
  time.
  - Identical-IR exhaust channels now share one convolution (16-251B audio
    thread 160 % -> 40 %).
  - Simulation frequency: 16-251B 3 kHz, 6-251D 10 kHz. Physics ~66-69 %, and
    loaded results are unchanged.
- Tools: `engine-sim-realtime-bench` (CPU budget per engine) and
  `engine-sim-audio-render` (offline WAV, no underruns; renders are in
  `C:\es\run\renders`).
- Turbo-off variants: `assets/alco_16_251b_no_turbo_main.mr` and
  `assets/alco_6_251d_no_turbo_main.mr` (MR input `turbo_enabled`; node
  `main_no_turbo`).
Audio redesign 2026-09-30 (details in docs/FAILURE_ATTEMPT_LOG.md):

- The seed's `ProceduralDieselAudio` injection of dp/dt clicks and the
  physics-rate turbo sine into the exhaust channels is removed.
- Diesel knock is a structure-borne layer driven by
  `sum(piston area * combustion dp/dt)` through a fixed structural band.
- Turbo sound is generated in the synthesizer at 44.1 kHz: blade-pass band
  noise plus a small tonal part, with amplitude ∝ sqrt(compressor power).
- Both layers are scaled by the level-control gain, which follows the exhaust
  signal only, so they never duck the engine.
- Only two global levels exist (`Synthesizer::AudioParameters`); no
  per-engine audio gains.
- Design rule: audio features must be broad-based (physics-driven, with global
  constants only).
- Loading the engine in the GUI needs the speed control raised while the dyno
  holds; dyno hold alone only motors it.

The Windows environment used Visual Studio 2022 x64 RelWithDebInfo, CMake
3.31.12, Boost 1.78, SDL2/SDL2_image through vcpkg, winflexbison3, Piranha
enabled, Discord disabled and DTV disabled.

## 12a. Local Windows workflow (user's machine)

The user builds the reconstructed source locally; CI remains the reference.

- Overlay repo (this branch): `C:\es\overlay`
- Reconstructed, fully patched source (from the review-build source zip): `C:\es\engine-sim`
- Build directory: `C:\es\engine-sim\build`; runnable layout: `C:\es\run` (`bin\`, `es\`, `assets\`, `basic\`, `bin\delta.conf` with `../basic` and `../assets`)
- Toolchain: VS 2022 x64, CMake 3.31.12 at `C:\es\tools\cmake-3.31.12-windows-x86_64\bin\cmake.exe`, Boost 1.78 at `C:\local\boost_1_78_0`, SDL2/SDL2_image via `C:\vcpkg`, winflexbison3.
- Every new PowerShell session needs, before configure/build:
  `$cmake=...cmake.exe; $env:SDL2DIR=$env:SDL2IMAGEDIR="C:\vcpkg\installed\x64-windows"; $env:BOOST_ROOT="C:\local\boost_1_78_0"; $env:BOOST_LIBRARYDIR="C:\local\boost_1_78_0\lib64-msvc-14.3"`
- Build: `& $cmake --build C:\es\engine-sim\build --config RelWithDebInfo --parallel --target <targets>`; then copy the `.exe` files to `C:\es\run\bin`. `C:\es\run\assets` and `C:\es\run\es` are separate copies: after changing an `.mr` template, also copy it to the same relative path under `C:\es\run`. Run headless tools from `C:\es\run` (they resolve `es\` and `assets\` from the working directory).
- Source of truth: edit the template under `C:\es\overlay\patches\forced_induction_v1\<path>` first, then copy it to `C:\es\engine-sim\<path>` (`robocopy C:\es\overlay\patches\forced_induction_v1 C:\es\engine-sim /E`). Never leave a change only in `C:\es\engine-sim`; commit it in the overlay with a failure-log entry as usual.
- Pushing this branch starts the Windows CI workflow (currently `REVIEW_BUILD_ONLY=1`); use `[skip ci]` for documentation-only commits.
- Save tool output as UTF-8 (`... 2>&1 | Out-File -Encoding utf8 file.txt`); `*>` redirection in Windows PowerShell writes UTF-16.
- Choosing an engine: the GUI takes no arguments; it compiles `C:\es\run\assets\main.mr` and reloads it on Enter. `C:\es\run\run-mr.cmd <file.mr> [-NoLaunch] [-Node name] | -List` (source: `tools/local/run-mr.ps1`) rewrites main.mr in the stock form (engine_sim.mr, default theme, import, `main()`), copies files from outside assets into `assets\custom\`, and starts the GUI from `bin`. Script compile errors go to `C:\es\run\bin\error_log.log`.
- Telemetry: the GUI and `engine-sim-cylinder-probe` write `C:\es\run\logs\telemetry_<engine>_<timestamp>.log`: HEADER lines, then one `SAMPLE key=value ...` line per 0.5 s of simulated time, plus `EVENT` lines for starter, ignition, dyno, gear and speed-control changes. Ask the user for the newest log when interpreting a GUI session.
- The Production-template pins in `tools/apply_forced_induction_v1.py` still apply: any authorized production change must update its pin in the same commit.

## 13. File map

- patches/forced_induction_v1/include/telemetry_log.h and src/telemetry_log.cpp —
  read-only run logger; src/engine_sim_application.cpp and its header call it.
- patches/forced_induction_v1/test/alco_251b_cylinder_probe.cpp — per-cylinder
  combustion/blowdown probe (`--check` proves bit-identity with the stock
  simulator).

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
