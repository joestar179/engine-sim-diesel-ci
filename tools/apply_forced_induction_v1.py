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
    "scripting/include/exhaust_system_node.h": "7ae7e4bf9d5e8aa66defff9031d62a925791b29c5c949e3a0d6542aa61cfd690",
    "scripting/src/compiler.cpp": "71314012594c46ee8b6151abdf8170cec88ed1cb1d5a2dcbc7bf95a17bbeed3e",
    "es/objects/objects.mr": "7b371b5fc5e2acf80e64d40a367fc0145b10239d497ae0b32392f398d6a40bea",
    "test/runtime_engine_smoke.cpp": "84e9e4027ef24029577979c41612165ce4811856dafd4a9b1acc924c72e32152",
    "include/engine_sim_application.h": "fb011cea2921d8355156a813d0e6fccfb67b0fff04c72fbc9a7658bdb662d8ef",
    "src/engine_sim_application.cpp": "50ea701ce25b010dc20a971a1aa3273a0b26bf3f837d8f504da00725229afec2",    "include/fuel_rack_governor_model.h": "dab343ade745cf38672b10cfb35c0eee0feb5c404b596687ae5c081a96a945c3",
    "src/gauge.cpp": "74ecf75be7758d3f3e12894d2f1ea2f32fa7008e738e790637289c7b9f6afe52",
    "src/synthesizer.cpp": "8ec3421e98b32f811cb3fcc8453ed30de77fef1fe4fc6a2fcb69421175ae34e8",
    "include/synthesizer.h": "a6320d59eb2f5cebf12ccea40fc73c9ba9f80944aa51fa236553a840b6353e54",
    "assets/engines/alco/alco_251d_diesel_turbo.mr": "c007e5d953a64a9f35524e4e86910e4a52d1c787ad1dfef89679677a50e0d0da",
    "src/fuel_rack_governor_model.cpp": "53278710f70970d78e28f4edc958ceaf146cce888d425ee8f2534390eacf2b41",
    "scripting/include/throttle_nodes.h": "417bf240163ddf3d6498aa047f412ef91759f648ca075a9f6e509c6e6c355c9b",
}

# The public v0.1.14a script library uses engine_channel at the three native
# action boundaries below. The pinned native donor predates that library
# correction, so install it as a separate readable compatibility patch rather
# than pretending that the closed v0.1.14a native source is available.
EXPECTED_COMPAT_BASELINE = {
    "es/actions/actions.mr": "95510e0e7e9252a28c02a2f07da9e31e9ffcadd5e32666f5e1170c0932e33d49",
}

ACTION_COMPAT_REPLACEMENTS = (
    (
        """public node set_engine => __engine_sim__set_engine {
    input engine [engine];
}""",
        """private node _set_engine => __engine_sim__set_engine {
    input engine [engine_channel];
}

public node set_engine {
    input engine;
    _set_engine(engine: engine)
}""",
    ),
    (
        """public node _add_crankshaft => __engine_sim__add_crankshaft {
    input crankshaft [crankshaft];
    input engine [engine];
}""",
        """public node _add_crankshaft => __engine_sim__add_crankshaft {
    input crankshaft [crankshaft];
    input engine [engine_channel];
}""",
    ),
    (
        """private node _add_ignition_module => __engine_sim__add_ignition_module {
    input ignition_module [ignition_module];
    input engine [engine];
}""",
        """private node _add_ignition_module => __engine_sim__add_ignition_module {
    input ignition_module [ignition_module];
    input engine [engine_channel];
}""",
    ),
)

# Production templates are pinned to their accepted content (71fff99, then the
# 2026-09-30 L-bank phasing, compressor passive-flow, turbo-volume momentum and
# telemetry-log changes). Diagnostic gates
# may add tests and build wiring but must not alter the code under test.
PINNED_PRODUCTION_TEMPLATES = {
    "assets/alco_16_251b_main.mr": "de603c279f374f62bd5b47ef0edebbabfbe083b01bdc2ab057620ff1af4c61f4",
    "assets/engines/alco/alco_16_251b_native.mr": "44467dc0b251104149c55e820652c3aee6dc6c58cdf190bdfdf9e08ceb34c71e",
    "es/objects/objects.mr": "5cfcf48a6873de4d3b3c0694765569f3c2225ea1c220ccad58e81e811ec04da8",
    "include/combustion_chamber.h": "907f66a94631211068bdcd1834d76188576df4dc92564be947685131e1a67017",
    "include/engine.h": "999e6bc481c2c0db03131a81caa90f213c0146c4b12783c7e3c75df876c4ab73",
    "include/exhaust_system.h": "050d489bb2fc579b0bf350b14797e55bb26e902908f941069d0b2d275e42df90",
    "include/forced_induction_system.h": "57c6bc6a9084bdda1f99d60851a3c6ea53ec0059be36610a8a5e8a151b68ce22",
    "include/intake.h": "4053725fe40e38e69c0bdcad93577a9022e9060e97fb192dc15c3ae8a4b169b3",
    "include/turbocharger_model.h": "37e92d401cca5f94073e858ebab134d16a1f1fd528e72563c63ea2abcb7fbece",
    "scripting/include/engine_node.h": "42e3b1b1a51c91e9f5dd7b7eec0ff75da47c06dd2ac141b16783e204baf249d4",
    "scripting/include/exhaust_system_node.h": "d5f3436f866d9f3608efcd808aa61d079e0eb6f207a9134c046b0a0940755c3d",
    "scripting/src/compiler.cpp": "fb6566d847ab4486d68ee39e813753cc5c0b9d52c7c5b988bb4fbd43ec02eeb0",
    "src/combustion_chamber.cpp": "b039c76a1ac59e88b073c2e34e76c0d2f5a6f9eaa0410942c075c2caeb98cd0a",
    "src/engine.cpp": "6e3e1ebee9bafd11791d01522f5934b375d7f9aa3abfc12a75e67527d004ef16",
    "src/exhaust_system.cpp": "538d16413843ef95686e6dd64ac138a0cc8f3f7d45e2f8970b707e705f8c8a51",
    "src/forced_induction_system.cpp": "f36e9e0c2431ec687cea25f846ebee64d9f4082355d62e423cf0dd9318890765",
    "src/intake.cpp": "4e8608837b82dd141e0f90a6f6f0580c8a4ea1c9fe8d2c2378a87497b8ec9792",
    "src/piston_engine_simulator.cpp": "c93f52504c6da1ea2cf0c855c6736a34ce2ceea0e2692a13ca3c8689601b71b0",
    "src/turbocharger_model.cpp": "389ed04821b31b392a660735a6a1d91949b6922d20de6e5c2dfbf96677f95fd1",
    "include/telemetry_log.h": "d3776a08765e4335b8d27b392bcda73ac70ae72f53fb6d951da69fa186aeed63",
    "src/telemetry_log.cpp": "62d328cacfa7896c2a8bb57119eb1d5e9b7824183f111e2e6d940c5d8576400c",
    "include/engine_sim_application.h": "f9015d5bd79c045519d80e34a00f123b60f2b1c925aed43ccb19879159c1afce",
    "src/engine_sim_application.cpp": "27bf9d1262223739fc1a638c8153e0dada6ed920d211f45bdb2993081888d91a",
    "include/fuel_rack_governor_model.h": "7b19852157d41f02df3f125801a13ec1f2bcd8fda219be81135657d32fa6cd27",
    "src/fuel_rack_governor_model.cpp": "cf7956417510ed17634a6a957a0f3ccf0fb04f4f221d049bcc1fe5e2f9cfea73",
    "scripting/include/throttle_nodes.h": "37df476feece66b6df48b40bd27c19d531a4c061829a555432f6a4841b325987",
    "src/gauge.cpp": "423436a81838a47929bb6955275716451be952455bbe2dde774e6b27193be797",
    "src/synthesizer.cpp": "1d458d0046d59ccacb4e12fa3d4ea15f3926e24a01c3c45314dbb38935253486",
    "include/synthesizer.h": "d3a136ad8a563d419e1dc26888d67b82b40bc51ed4aa09429ea5e04b1c83ba16",
    "assets/engines/alco/alco_251d_diesel_turbo.mr": "03221a4a6247c8cdea1c4cbad8bf3921ff5382a4b01d8fa7a3107fbe4eee7ce2",
}

NEW_FILES = {
    "assets/alco_16_251b_no_turbo_main.mr",
    "assets/alco_6_251d_no_turbo_main.mr",
    "test/realtime_budget_bench.cpp",
    "test/audio_render.cpp",
    "include/telemetry_log.h",
    "src/telemetry_log.cpp",
    "include/forced_induction_system.h",
    "src/forced_induction_system.cpp",
    "assets/alco_16_251b_main.mr",
    "assets/engines/alco/alco_16_251b_native.mr",
    "test/alco_251b_cylinder_probe.cpp",
    "test/alco_251b_governor_observability.cpp",
    "test/alco_251b_loaded_transient_validation.cpp",
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
        "m_chargePlenum.dissipateVelocity(dt, 0.0);",
        "flow.k_flow = m_passiveCompressorFlowK;",
    ],
    "include/exhaust_system.h": [
        "This is always the original downstream ExhaustSystem volume",
        "getBackflowAtmosphericMixing()",
        "GasSystem m_system;",
    ],
    "src/exhaust_system.cpp": [
        "std::clamp(params.backflowAtmosphericMixing, 0.0, 1.0)",
        "airMix.p_o2 = 0.25 * m_backflowAtmosphericMixing",
        "m_flow = GasSystem::flow(flowParams)",
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
        "getTurboScrollIndex()",
        "exhaustScrollIndices.push_back(",
    ],
    "scripting/include/exhaust_system_node.h": [
        "getTurboScrollIndex()",
        'addInput("backflow_atmospheric_mixing"',
        'addInput("turbo_scroll_index"',
    ],
    "scripting/src/compiler.cpp": [
        "Runtime execution failed",
        "m_program.getRuntimeError()",
    ],
    "es/objects/objects.mr": [
        "input backflow_atmospheric_mixing: 0.0",
        "backflow_atmospheric_mixing: backflow_atmospheric_mixing",
        "input turbo_scroll_index: -1",
        "turbo_scroll_index: turbo_scroll_index",
    ],
    "assets/engines/alco/alco_16_251b_native.mr": [
        "compression_ignition: true",
        "throttle: diesel_governor(",
        "cylinder_bank bank_R(bank_params, angle:  bank_angle / 2.0)",
        "tdc: 90 * units.deg + (bank_angle / 2.0)",
        "k_p: 3.0,",
        "crank_rack_limit: 0.35",
        "simulation_frequency: 3000,",
        "public node main_no_turbo {",
        "turbo_inlet_channel_count: 4",
        "turbo_scroll_index: 0",
        "turbo_scroll_index: 3",
        "aftercooler_enabled: turbo_enabled",
        "input turbo_enabled: true;",
        "exhaust_system: exhaust_r_a",
        "exhaust_system: exhaust_l_b",
    ],
    "assets/alco_16_251b_main.mr": [
        'import "engines/alco/alco_16_251b_native.mr"',
        "main()",
    ],
    "test/alco_251b_loaded_transient_validation.cpp": [
        "stockSiCompatibility(",
        "v014a-stock-si-load",
        "requireNative251B(",
        "loadedCausalChain(",
        "stableRelease(",
        "GATE6_FAIL classification=",
        "runtimeErrorDetails()",
        "std::copysign(",
        "setSpeedControl(IdleCommand)",
    ],
    "test/alco_251b_governor_observability.cpp": [
        "struct ShadowGovernor",
        'recorder.run("release", ReleaseCommand, 300, false, true);',
        "forwardSign * units::rpm(HeldRpm)",
        "GATE6B_CLASSIFICATION=",
        "GATE6B_FAIL classification=",
    ],
    "test/forced_induction_invariant_tests.cpp": [
        "ForcedInductionDisabledPathInvariant",
        "ForcedInductionCompressorInvariant",
        "ForcedInductionTurbineInvariant",
        "ForcedInductionEnergyInvariant",
        "ForcedInductionOptionalDeviceInvariant",
        "ForcedInductionMultiGroupInvariant",
        "ForcedInductionMassBalanceInvariant",
        "backflow.backflowAtmosphericMixing = 1.0",
        "EXPECT_GT(mixedBoundary.getSystem()->n_o2(), 0.0)",
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
        "engine-sim-realtime-bench",
        "engine-sim-audio-render",
        "engine-sim-integration-validation",
        "AlcoIntegrationValidation.NullNaturallyAspiratedSi",
        "AlcoIntegrationValidation.AftercoolerActualCharge",
        "engine-sim-loaded-transient-validation",
        "Alco251BLoadedTransient.CausalChain",
        "Alco251BLoadedTransient.StableRelease",
        "V014aCompatibility.StockSiLoads",
        "engine-sim-governor-observability",
        "Alco251BObservability.Governor",
        "engine-sim-cylinder-probe",
    ],
    "src/fuel_rack_governor_model.cpp": [
        "m_parameters.k_p * (target - speed) / target",
        "m_starting",
    ],
    "scripting/include/throttle_nodes.h": [
        'addInput("k_p", &m_parameters.k_p);',
        'addInput("crank_rack_limit", &m_parameters.crankRackLimit);',
    ],
    "src/gauge.cpp": [
        "constexpr float MaxNeedleStep = 1.0f / 120.0f;",
    ],
    "src/synthesizer.cpp": [
        "m_filters[index].convolutionOwner = j;",
    ],
    "assets/engines/alco/alco_251d_diesel_turbo.mr": [
        "simulation_frequency: 10000,",
        "public node main_no_turbo {",
    ],
    "src/telemetry_log.cpp": [
        "void TelemetryLog::writeSnapshot()",
    ],
    "src/engine_sim_application.cpp": [
        "m_telemetryLog.sampleStep();",
        "m_telemetryLog.endFrame();",
    ],
    "test/alco_251b_cylinder_probe.cpp": [
        "class ProbeSimulator : public PistonEngineSimulator",
        "struct ChamberPeek : CombustionChamber",
        "sim->m_dyno.m_enabled = false;",
    ],
    "es/actions/actions.mr": [
        "private node _set_engine => __engine_sim__set_engine",
        "input engine [engine_channel]",
        "_set_engine(engine: engine)",
    ],
}

FORBIDDEN_POSTCONDITIONS = {
    "include/exhaust_system.h": ["m_tailpipe", "setTurboMode", "getTailpipeSystem"],
    "src/exhaust_system.cpp": ["m_tailpipe", "m_turbineExtractableEnergy"],
    "include/intake.h": ["setSupplyState("],
    "src/intake.cpp": ["recordTurboCompressorFlow", "compressorOperatingPoint("],
    "include/engine.h": ["m_turbocharger", "m_turboExhaustMoles", "m_lastTurboOutput"],
    "src/engine.cpp": ["recordTurboExhaustFlow", "recordTurboCompressorFlow"],
    "assets/engines/alco/alco_16_251b_native.mr": [
        "cylinder_bank bank_R(bank_params, angle: -bank_angle / 2.0)",
        "Diesel combustion surrogate",
        "Stock Engine Simulator cannot model compressor pressure ratio",
        "exhaust_system: exhaust,",
        "throttle: governor(",
        "\nmain()\n",
    ],
    "es/actions/actions.mr": [
        "public node set_engine => __engine_sim__set_engine",
    ],
    "test/alco_251b_loaded_transient_validation.cpp": [
        "m_dyno.m_rotationSpeed = units::rpm(600)",
        "setSpeedControl(LowCommand);\n        sequence.release",
    ],
    "test/alco_251b_governor_observability.cpp": [
        "m_dyno.m_rotationSpeed = units::rpm(HeldRpm)",
    ],
}


def digest(path: Path) -> str:
    # The accepted patch scripts use Path.write_text(), which materializes LF
    # on Linux and CRLF on Windows. Normalize only that platform distinction;
    # every other byte remains part of the pinned content precondition.
    return sha256(path.read_bytes().replace(b"\r\n", b"\n")).hexdigest()


def fail(message: str) -> None:
    raise RuntimeError(message)


def verify_production_templates(bundle: Path) -> None:
    template_root = bundle / "patches" / "forced_induction_v1"
    for relative, expected in PINNED_PRODUCTION_TEMPLATES.items():
        path = template_root / relative
        if not path.is_file():
            fail(f"precondition failed: missing pinned production template {relative}")
        actual = digest(path)
        if actual != expected:
            fail(
                f"precondition failed: production template {relative} differs from "
                f"its 71fff99 content (expected {expected}, found {actual})"
            )


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

    for relative, expected in EXPECTED_COMPAT_BASELINE.items():
        path = source / relative
        if not path.is_file():
            fail(f"precondition failed: missing compatibility baseline file {relative}")
        actual = digest(path)
        if actual != expected:
            fail(
                f"precondition failed for {relative}: expected compatibility-baseline "
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

    actions = source / "es/actions/actions.mr"
    text = actions.read_text(encoding="utf-8")
    for old, new in ACTION_COMPAT_REPLACEMENTS:
        count = text.count(old)
        if count != 1:
            fail(
                "compatibility patch precondition failed for es/actions/actions.mr: "
                f"expected one anchored action boundary, found {count}"
            )
        text = text.replace(old, new, 1)
    actions.write_text(text, encoding="utf-8")


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
    verify_production_templates(bundle)
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
