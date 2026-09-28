#!/usr/bin/env python3
"""Install the locked Generic Forced-Induction V1 source as readable files.

This stage intentionally replaces the accepted generated turbo core only after
the legacy baseline has been reconstructed. Every replaced text file is pinned
by a SHA-256 precondition after CRLF-to-LF normalization, and the resulting
topology is checked before success.
"""

from hashlib import sha256
from pathlib import Path
import shutil
import subprocess
import sys


PINNED_UPSTREAM = "56725cc012581282567900b15871018d55b7ab42"

EXPECTED_BASELINE = {
    "CMakeLists.txt": "7f45692c36bfe92878942b463e61d103da0feba9046c0d386165dbd47cac9fda",
    "include/turbocharger_model.h": "18f54c21ad794331a86d7d2f7d9872061c0640c35f8c8e1bfef0c78da29b26cf",
    "src/turbocharger_model.cpp": "c2f82dc0d87d89c32c9260f1aa972db63c5422a3b67b99a2da486d8e4bf4c9ee",
    "include/intake.h": "0a98115a126ecda6455d206a8e20faf04f036b740fb1395c2505668aaf43a57d",
    "src/intake.cpp": "a2200719ed9db0ae5e9a4b922cf13b9348e8bc822119170730d52f5956896b62",
    "include/exhaust_system.h": "6f75fa0ce903bc4e49a6e1aad86a9c682a04bcb4363b8b173cf7dbaa7373ce2a",
    "src/exhaust_system.cpp": "19313b82a7846f8a46760bc3e2ce3fe4107cf26ef2cd048029c46f3b00b2983f",
    "include/engine.h": "e28025fbdda925429b7cf914688c1c83d84e6a9c3e8bb39997168964a7681b8c",
    "src/engine.cpp": "412b14470a2c57b11889d26946b93273d6f9908337015f64532d96690fadc0f0",
    "include/combustion_chamber.h": "6859a8cc8b6ff6e161dcf76a14c47f427cda0a2ac92c05df62e5ec730fe1b64c",
    "src/combustion_chamber.cpp": "a3d83f572cfc8376d020eb4d9e44a3f423e121fce8b9b0f3be54a95f97abb8e1",
    "src/piston_engine_simulator.cpp": "daab65c2ee3006ca97afdffd1ef97e5ad388524282ad2c74f93d4d3299592e72",
    "scripting/include/engine_node.h": "14fc19f2dc1dc1a53053366081d3fae4f57ab0bed872c693378204b6f5a0d536",
    "es/objects/objects.mr": "7b371b5fc5e2acf80e64d40a367fc0145b10239d497ae0b32392f398d6a40bea",
    "test/runtime_engine_smoke.cpp": "84e9e4027ef24029577979c41612165ce4811856dafd4a9b1acc924c72e32152",
}

NEW_FILES = {
    "include/forced_induction_system.h",
    "src/forced_induction_system.cpp",
    "test/alco_integration_validation.cpp",
    "test/forced_induction_invariant_tests.cpp",
    "test/forced_induction_runtime_smoke_tests.cpp",
}

REQUIRED_POSTCONDITIONS = {
    "include/forced_induction_system.h": [
        "class TurboGroup",
        "std::vector<TurboGroup> m_groups",
        "std::vector<GasSystem> m_preTurbine",
    ],
    "src/forced_induction_system.cpp": [
        "boundedTransfer(",
        "processTurbineFlow(",
        "post->changeEnergy(-extracted)",
        "m_rotatingAssembly.advanceShaft(",
    ],
    "include/exhaust_system.h": [
        "This is always the original downstream ExhaustSystem volume",
        "GasSystem m_system;",
    ],
    "src/combustion_chamber.cpp": [
        "m_engine->getExhaustDestination(exhaust)",
    ],
    "src/piston_engine_simulator.cpp": [
        "m_engine->processForcedInduction(fluidTimestep)",
    ],
    "scripting/include/engine_node.h": [
        "turbo_pre_turbine_volume",
        "compressor_bypass_recirculates",
        "vgt_time_constant",
    ],
    "test/forced_induction_invariant_tests.cpp": [
        "ForcedInductionDisabledPathInvariant",
        "ForcedInductionCompressorInvariant",
        "ForcedInductionTurbineInvariant",
        "ForcedInductionEnergyInvariant",
        "ForcedInductionOptionalDeviceInvariant",
        "ForcedInductionMultiGroupInvariant",
        "ForcedInductionMassBalanceInvariant",
    ],
    "test/forced_induction_runtime_smoke_tests.cpp": [
        "NaturallyAspiratedSiKeepsOriginalStablePath",
        "ThrottledSiAirDeliveryRespondsToThrottle",
        "UnthrottledAirPathTransitionsFromPassiveToPoweredFlow",
        "ParallelGroupsRemainIndependentWhileFeedingSharedIntake",
        "TwinScrollRoutingRetainsPulseSeparationOnOneShaft",
    ],
    "test/alco_integration_validation.cpp": [
        "nullNaturallyAspiratedSi(",
        "alcoPassiveCranking(",
        "alcoExhaustToScroll(",
        "alcoTurbineAcceleratesShaft(",
        "alcoCompressorChangesCharge(",
        "alcoFuelControlSeparation(",
        "alcoAftercoolerActualCharge(",
        "GATE5_FAIL classification=",
    ],
    "CMakeLists.txt": [
        "engine-sim-integration-validation",
        "AlcoIntegrationValidation.NullNaturallyAspiratedSi",
        "AlcoIntegrationValidation.AftercoolerActualCharge",
    ],
}

FORBIDDEN_POSTCONDITIONS = {
    "include/exhaust_system.h": ["m_tailpipe", "setTurboMode", "getTailpipeSystem"],
    "src/exhaust_system.cpp": ["m_tailpipe", "m_turbineExtractableEnergy"],
    "include/intake.h": ["setSupplyState("],
    "src/intake.cpp": ["recordTurboCompressorFlow", "compressorOperatingPoint("],
    "include/engine.h": ["m_turbocharger", "m_turboExhaustMoles", "m_lastTurboOutput"],
    "src/engine.cpp": ["recordTurboExhaustFlow", "recordTurboCompressorFlow"],
}


def digest(path: Path) -> str:
    # The accepted patch scripts use Path.write_text(), which materializes LF
    # on Linux and CRLF on Windows. Normalize only that platform distinction;
    # every other byte remains part of the pinned content precondition.
    return sha256(path.read_bytes().replace(b"\r\n", b"\n")).hexdigest()


def fail(message: str) -> None:
    raise RuntimeError(message)


def verify_preconditions(source: Path) -> None:
    if not (source / "CMakeLists.txt").is_file():
        fail("target is not an Engine Simulator source tree")

    try:
        head = subprocess.check_output(
            ["git", "-C", str(source), "rev-parse", "HEAD"], text=True
        ).strip()
    except (OSError, subprocess.CalledProcessError) as exc:
        fail(f"could not verify target upstream commit: {exc}")
    if head != PINNED_UPSTREAM:
        fail(f"wrong upstream commit: expected {PINNED_UPSTREAM}, found {head}")

    for relative, expected in EXPECTED_BASELINE.items():
        path = source / relative
        if not path.is_file():
            fail(f"precondition failed: missing accepted baseline file {relative}")
        actual = digest(path)
        if actual != expected:
            fail(
                f"precondition failed for {relative}: expected accepted-baseline "
                f"SHA-256 {expected}, found {actual}"
            )

    for relative in NEW_FILES:
        if (source / relative).exists():
            fail(f"precondition failed: new V1 file already exists: {relative}")


def apply(source: Path, bundle: Path) -> None:
    template_root = bundle / "patches" / "forced_induction_v1"
    replacements = set(EXPECTED_BASELINE) | NEW_FILES
    for relative in sorted(replacements):
        template = template_root / relative
        destination = source / relative
        if not template.is_file():
            fail(f"readable V1 template missing: {relative}")
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(template, destination)


def verify_postconditions(source: Path, bundle: Path) -> None:
    template_root = bundle / "patches" / "forced_induction_v1"
    replacements = set(EXPECTED_BASELINE) | NEW_FILES
    for relative in replacements:
        actual = source / relative
        template = template_root / relative
        if not actual.is_file() or digest(actual) != digest(template):
            fail(f"post-condition failed: generated source differs from readable template: {relative}")

    for relative, required in REQUIRED_POSTCONDITIONS.items():
        text = (source / relative).read_text(encoding="utf-8")
        for token in required:
            if token not in text:
                fail(f"post-condition failed: {relative} lacks required token {token!r}")

    for relative, forbidden in FORBIDDEN_POSTCONDITIONS.items():
        text = (source / relative).read_text(encoding="utf-8")
        for token in forbidden:
            if token in text:
                fail(f"post-condition failed: {relative} retains forbidden legacy shortcut {token!r}")

    core = (source / "src/forced_induction_system.cpp").read_text(encoding="utf-8")
    if "chargePressure =" in core or "setSupplyState(" in core:
        fail("post-condition failed: forced-induction core imposes a boost pressure boundary")

    print("Generic Forced-Induction V1 readable source applied; topology post-conditions PASS")


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: apply_forced_induction_v1.py /path/to/engine-sim", file=sys.stderr)
        return 2
    source = Path(sys.argv[1]).resolve()
    bundle = Path(__file__).resolve().parents[1]
    verify_preconditions(source)
    apply(source, bundle)
    verify_postconditions(source, bundle)
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except RuntimeError as exc:
        print(f"forced-induction V1 application failed: {exc}", file=sys.stderr)
        raise SystemExit(1)
