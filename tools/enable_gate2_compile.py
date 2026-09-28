#!/usr/bin/env python3
"""Enable the bounded Gate 2 core-compile path in reconstructed Windows CI.

The deterministic seed owns tools/windows_ci.ps1, so this readable adapter is
applied after seed reconstruction.  It accepts exactly the Gate 1-verified
script, skips baseline test execution in Gate 2, builds only the core,
scripting, and smoke-test targets, verifies their outputs, then exits before
any test execution, simulation, GUI build, packaging, or tuning.
"""

from hashlib import sha256
from pathlib import Path
import sys


EXPECTED_INPUT_SHA256 = (
    "48f06ae84f2a7664e9af2981bfff6ba01a885b85e74c9bfd62a0da3c85db7c8e"
)


BASELINE_BLOCK = """Write-Host '=== Establish pristine upstream test baseline ==='
& $cmakeExe -S $source -B $baselineBuild @commonConfigure
if ($LASTEXITCODE -ne 0) { throw 'baseline configure failed' }
& $cmakeExe --build $baselineBuild --config RelWithDebInfo --target engine-sim-test --parallel
if ($LASTEXITCODE -ne 0) { throw 'baseline test build failed' }
$baselineXml = Join-Path $logs 'baseline.xml'
& $ctestExe --test-dir $baselineBuild -C RelWithDebInfo --output-on-failure --output-junit $baselineXml
$baselineCtestCode = $LASTEXITCODE
Write-Host "Pristine baseline CTest exit code: $baselineCtestCode (failures are compared, not blindly rejected)"
"""


GATED_BASELINE_BLOCK = """if ($env:CORE_COMPILE_ONLY -ne '1') {
    Write-Host '=== Establish pristine upstream test baseline ==='
    & $cmakeExe -S $source -B $baselineBuild @commonConfigure
    if ($LASTEXITCODE -ne 0) { throw 'baseline configure failed' }
    & $cmakeExe --build $baselineBuild --config RelWithDebInfo --target engine-sim-test --parallel
    if ($LASTEXITCODE -ne 0) { throw 'baseline test build failed' }
    $baselineXml = Join-Path $logs 'baseline.xml'
    & $ctestExe --test-dir $baselineBuild -C RelWithDebInfo --output-on-failure --output-junit $baselineXml
    $baselineCtestCode = $LASTEXITCODE
    Write-Host "Pristine baseline CTest exit code: $baselineCtestCode (failures are compared, not blindly rejected)"
}
"""


SOURCE_CAPTURE_TAIL = """    Write-Host 'Captured exact current turbo core source.'
    exit 0
}
"""


GATE2_BLOCK = SOURCE_CAPTURE_TAIL + r'''

if ($env:CORE_COMPILE_ONLY -eq '1') {
    Write-Host '=== Gate 2: configure enhanced core only ==='
    $configureLog = Join-Path $logs 'gate2-configure.log'
    $previousErrorActionPreference = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    & $cmakeExe -S $source -B $enhancedBuild @commonConfigure 2>&1 |
        Tee-Object -FilePath $configureLog
    $configureCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($configureCode -ne 0) { throw "Gate 2 configure failed with exit code $configureCode" }

    Write-Host '=== Gate 2: compile and link scoped targets only ==='
    $gate2Targets = @(
        'engine-sim',
        'engine-sim-script-interpreter',
        'engine-sim-test',
        'engine-sim-script-smoke',
        'engine-sim-runtime-smoke'
    )
    $buildLog = Join-Path $logs 'gate2-build.log'
    $ErrorActionPreference = 'Continue'
    & $cmakeExe --build $enhancedBuild --config RelWithDebInfo --target $gate2Targets --parallel 2>&1 |
        Tee-Object -FilePath $buildLog
    $buildCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($buildCode -ne 0) { throw "Gate 2 scoped build failed with exit code $buildCode" }

    $expectedOutputs = @(
        (Join-Path $enhancedBuild 'RelWithDebInfo\engine-sim.lib'),
        (Join-Path $enhancedBuild 'RelWithDebInfo\engine-sim-script-interpreter.lib'),
        (Join-Path $enhancedBuild 'RelWithDebInfo\engine-sim-test.exe'),
        (Join-Path $enhancedBuild 'RelWithDebInfo\engine-sim-script-smoke.exe'),
        (Join-Path $enhancedBuild 'RelWithDebInfo\engine-sim-runtime-smoke.exe')
    )
    foreach ($output in $expectedOutputs) {
        if (-not (Test-Path $output)) { throw "Gate 2 expected build output missing: $output" }
    }

    $evidence = Join-Path $logs 'gate2-core-compile.txt'
    @(
        'Gate 2 core compile and link: PASS',
        "Pinned upstream: $actualRoot",
        'Configuration: Visual Studio 2022 x64 RelWithDebInfo',
        "Targets: $($gate2Targets -join ', ')",
        'Executed tests/simulations: none',
        'GUI/app target: not built',
        'Runtime packaging: not run'
    ) | Set-Content -Path $evidence -Encoding utf8

    Write-Host '=== GATE 2 CORE COMPILE AND LINK PASSED ==='
    exit 0
}
'''


def normalized_digest(text: str) -> str:
    return sha256(text.replace("\r\n", "\n").encode("utf-8")).hexdigest()


def replace_exactly_once(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"precondition failed: expected one {label} anchor, found {count}")
    return text.replace(old, new, 1)


def verify_postconditions(text: str) -> None:
    required = (
        "if ($env:CORE_COMPILE_ONLY -ne '1')",
        "if ($env:CORE_COMPILE_ONLY -eq '1')",
        "'engine-sim-script-interpreter'",
        "'engine-sim-runtime-smoke'",
        "$ErrorActionPreference = 'Continue'",
        "$ErrorActionPreference = $previousErrorActionPreference",
        "Gate 2 core compile and link: PASS",
        "GATE 2 CORE COMPILE AND LINK PASSED",
    )
    for token in required:
        if token not in text:
            raise RuntimeError(f"post-condition failed: missing {token!r}")

    start = text.index("if ($env:CORE_COMPILE_ONLY -eq '1')")
    end_anchor = "New-Item -ItemType Directory -Force -Path (Join-Path $source 'assets\\engines\\alco')"
    end = text.find(end_anchor, start)
    if end < 0:
        raise RuntimeError("post-condition failed: could not delimit Gate 2 branch")
    gate2 = text[start:end]
    forbidden = (
        "engine-sim-app",
        "& $ctestExe",
        "engine-sim-script-smoke.exe' stock",
        "package_runtime.ps1",
        "Compress-Archive -Path (Join-Path $runtime",
    )
    for token in forbidden:
        if token in gate2:
            raise RuntimeError(f"post-condition failed: Gate 2 contains forbidden action {token!r}")


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: enable_gate2_compile.py /path/to/tools/windows_ci.ps1", file=sys.stderr)
        return 2

    target = Path(sys.argv[1]).resolve()
    if not target.is_file():
        raise RuntimeError(f"precondition failed: missing reconstructed CI script: {target}")

    text = target.read_text(encoding="utf-8-sig")
    if normalized_digest(text) != EXPECTED_INPUT_SHA256:
        raise RuntimeError(
            "precondition failed: reconstructed Windows CI does not match "
            "the Gate 1-verified input"
        )

    text = text.replace("\r\n", "\n")
    text = replace_exactly_once(
        text, BASELINE_BLOCK, GATED_BASELINE_BLOCK, "baseline-build"
    )
    text = replace_exactly_once(
        text, SOURCE_CAPTURE_TAIL, GATE2_BLOCK, "source-capture tail"
    )
    verify_postconditions(text)
    target.write_text(text, encoding="utf-8", newline="\n")
    print("Gate 2 core-compile-only CI path enabled; post-conditions PASS")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except RuntimeError as exc:
        print(f"Gate 2 CI adapter failed: {exc}", file=sys.stderr)
        raise SystemExit(1)
