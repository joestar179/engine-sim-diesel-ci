# v0.1.14a Compatibility Baseline

## What is—and is not—the native baseline

Engine Simulator Community Edition v0.1.14a is a distribution release, not an
open-source native-code release. Its repository tag contains documentation and
tutorial material but not the application C++ source. Therefore this project
does **not** claim to contain or derive from v0.1.14a native source.

The deterministic native donor remains:

- repository: `ange-yaghi/engine-sim`;
- commit: `56725cc012581282567900b15871018d55b7ab42`;
- describe result: `v0.1.11a-6-g56725cc`.

CI reconstructs that pin, applies the accepted diesel/compression-ignition
changes, the accepted pre-V1 turbo work, and finally the readable Generic
Forced-Induction V1 overlay. This is a known composition; it is not an unknown
mix and it must not be described as v0.1.14a native code.

## Frozen official distribution evidence

- release: `engine-sim-v0.1.14a.zip`;
- source URL: `https://github.com/Engine-Simulator/engine-sim-community-edition/releases/download/v0.1.14a/engine-sim-v0.1.14a.zip`;
- archive SHA-256: `2fc1e7c2ad6a94af4bbe78e69907b57aad3d5ebda7d55563e24fa2e14707fbe6`;
- stock SI null reference: `assets/engines/kohler/kohler_ch750.mr`;
- stock SI SHA-256: `c729091fe9c6d3ad3761564850bfad437ea441fee7308a7e7c7ea226ad8b2196`.

The release contains no `.cpp`, `.h` or `.hpp` application source. The CI gate
downloads the public release, verifies both hashes, extracts only the stock SI
reference into the temporary reconstructed source tree, and does not commit the
distribution asset to this repository.

## Material script-library differences found

A line-ending-normalized comparison of the official `es/` library with the
reconstructed donor found material differences in:

- `es/actions/actions.mr`;
- `es/constants/units.mr`;
- `es/engine_sim_theme.mr` (official-only);
- `es/objects/objects.mr`;
- `es/part-library/parts/heads.mr`;
- `es/settings/application_settings.mr`.

The earlier names-only comparison of `objects.mr` was insufficient and must not
be treated as a compatibility proof.

## Supported compatibility change

The current failure was traced to one supported boundary difference. The
reconstructed object library emits `engine_channel`, while its older action
library consumed the obsolete `engine` type at three native actions. The
readable anchored transform in `tools/apply_forced_induction_v1.py` changes only:

- `set_engine` via a private native `_set_engine` accepting `engine_channel`;
- `_add_crankshaft` to accept `engine_channel`;
- `_add_ignition_module` to accept `engine_channel`.

Application requires the normalized pre-patch SHA-256
`95510e0e7e9252a28c02a2f07da9e31e9ffcadd5e32666f5e1170c0932e33d49`
and explicit post-condition tokens. The old direct `set_engine` declaration is
forbidden after application.

This is intentionally a compatibility subset, not a wholesale v0.1.14a API
port. Wankel actions, `k_32inH2O`, theme/settings changes and other features
whose native implementations do not exist at the pinned donor are out of scope.

## Validation order

Gate 6 is bounded and ordered:

1. load the exact official Kohler CH750 naturally aspirated SI reference;
2. assert spark ignition, forced induction disabled, and original exhaust
   routing;
3. only if that passes, run the two native 16-251B loaded-transient checks;
4. do not run the full suite, GUI/app target, packaging, boost tuning or power
   calibration.

Any failure is classified before another change. The interpreter-diagnostic
layer has already used both allowed attempts and may not receive another
variation of the same fix.
