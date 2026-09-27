# Failure-Loop Guard

Status: ACTIVE

This policy exists to prevent repeated speculative repair cycles that change the same layer without new evidence.

## 1. Attempt budget

For any single failure signature, allow at most **two repair attempts in the same layer**.

A failure signature is the combination of:
- failing stage;
- primary error text/class;
- affected subsystem/layer.

After two unsuccessful attempts:
1. stop making further fixes in that layer;
2. re-read the implementation and the last two failures;
3. classify the failure again from first principles;
4. change strategy or representation, not just parameters;
5. document the new hypothesis before another code change.

Examples:
- two payload-decoding failures => abandon payload transport, use readable source/patch files;
- two linker repairs for the same symbols => inspect generated source and build graph before another linker patch;
- two runtime-start failures with no combustion => instrument injection/combustion before changing starter torque.

## 2. No opaque source-patch transport

Generated C++/MR implementation patches must be committed as readable text files.

Forbidden for implementation patches:
- base64-encoded source payloads;
- gzip/zlib-compressed source payloads;
- split encoded payload parts;
- self-extracting patch blobs.

Exceptions:
- the deterministic CI seed archive may remain encoded because it is a binary bootstrap artifact, not an iterative source patch;
- the pre-policy legacy files `.ci/turbo_arch_patch.py` and `.ci/full_turbo_topology_patch.py` are temporarily grandfathered only so the accepted baseline remains reproducible. They may be converted to readable form, but no new opaque patch runner may be introduced.

## 3. Stage gates

Never proceed to a later gate when an earlier one failed.

Order:
1. patch/application integrity;
2. generated-source inspection;
3. configure;
4. compile;
5. link;
6. regression comparison;
7. script parse/execute;
8. runtime smoke;
9. physics/topology validation;
10. packaging.

A packaging failure is not an engine-physics failure.
A patch transport failure is not a compile failure.
A compile success is not runtime validation.

## 4. Mandatory evidence before another attempt

Every repair after a failure must cite at least one new piece of evidence:
- exact generated source;
- exact compiler/linker diagnostic;
- runtime telemetry;
- failing test output;
- package dependency audit.

If there is no new evidence, do not make another speculative code change.

## 5. Smallest-layer rule

Change only the layer implicated by the evidence.

Do not change:
- calibration to solve topology errors;
- starter torque to solve missing combustion;
- package dependencies to solve engine runtime behavior;
- turbo maps to solve incorrect gas-path ordering.

## 6. Representation escape hatch

If the mechanism used to deliver/apply a change fails twice, replace the mechanism.

Examples:
- encoded patch payload -> normal readable patch/source file;
- fragile string replacement -> structured file replacement or smaller anchored edits;
- single giant patch -> ordered small patches with post-conditions.

## 7. Post-condition checks

Every patch script must verify the state it claims to create.

At minimum:
- required anchors existed before modification;
- expected definitions exist after modification;
- forbidden legacy shortcut does not remain;
- patch exits nonzero if post-conditions fail.

## 8. Run polling rule

During CI validation:
- poll the same run; do not create a new run merely because it is slow;
- only create a new run after a concrete failure is diagnosed and a code change is committed;
- do not stack speculative commits while a prior run is still unresolved.

## 9. Stop condition

If three distinct strategies fail to solve the same root problem, stop implementation and produce a root-cause report before further changes.

That report must include:
- what was tried;
- why each attempt failed;
- what assumptions may be wrong;
- what evidence is still missing;
- the next proposed diagnostic, not another blind fix.

## 10. Current incident application

The generic turbo implementation transport has already failed more than twice using compressed/encoded payloads.

Therefore that strategy is retired.

The next implementation must use readable repository source/patch files and must pass patch-integrity/post-condition checks before compilation is allowed.


## 11. Enforcement boundary

CI automatically enforces repository-state rules such as banned opaque payload transports and retired mechanisms.

The two-attempt budget is a development-process rule because CI has read-only repository permissions and cannot safely mutate a cross-run attempt counter. Every repeated failure must therefore be entered in `docs/FAILURE_ATTEMPT_LOG.md`; if the same signature reaches attempt 2, the next entry must use a different strategy/layer or be a diagnostic-only step.
