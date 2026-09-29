#!/usr/bin/env python3
"""Enable bounded Gate 2 through Gate 6 paths in reconstructed Windows CI.

The deterministic seed owns tools/windows_ci.ps1, so this readable adapter is
applied after seed reconstruction.  It accepts exactly the Gate 1-verified
script. Gate 2 compiles only scoped targets. Gate 3 builds the unit-test target
and runs exactly the seven forced-induction architecture-invariant tests. Gate
4 runs exactly five generic synthetic runtime smokes. Gate 5 runs one real
upstream SI null and six ALCO integration checks. Gate 6 runs two loaded
transient checks against the native 16-251B reference. All paths exit before
the full regression suite, GUI build, packaging, or tuning.
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


GATED_BASELINE_BLOCK = """if ($env:CORE_COMPILE_ONLY -ne '1' -and $env:ARCHITECTURE_TESTS_ONLY -ne '1' -and $env:GENERIC_RUNTIME_SMOKE_ONLY -ne '1' -and $env:ALCO_INTEGRATION_ONLY -ne '1' -and $env:ALCO_251B_LOADED_TRANSIENT_ONLY -ne '1') {
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


GATED_BLOCKS = SOURCE_CAPTURE_TAIL + r'''

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

if ($env:ARCHITECTURE_TESTS_ONLY -eq '1') {
    Write-Host '=== Gate 3: configure architecture-invariant tests ==='
    $configureLog = Join-Path $logs 'gate3-configure.log'
    $previousErrorActionPreference = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    & $cmakeExe -S $source -B $enhancedBuild @commonConfigure 2>&1 |
        Tee-Object -FilePath $configureLog
    $configureCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($configureCode -ne 0) { throw "Gate 3 configure failed with exit code $configureCode" }

    Write-Host '=== Gate 3: build unit-test target only ==='
    $buildLog = Join-Path $logs 'gate3-build.log'
    $ErrorActionPreference = 'Continue'
    & $cmakeExe --build $enhancedBuild --config RelWithDebInfo --target engine-sim-test --parallel 2>&1 |
        Tee-Object -FilePath $buildLog
    $buildCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($buildCode -ne 0) { throw "Gate 3 test build failed with exit code $buildCode" }

    $testBinary = Join-Path $enhancedBuild 'RelWithDebInfo\engine-sim-test.exe'
    if (-not (Test-Path $testBinary)) { throw "Gate 3 expected test binary missing: $testBinary" }

    $testFilter = 'ForcedInduction.*Invariant'
    $listLog = Join-Path $logs 'gate3-test-list.log'
    $ErrorActionPreference = 'Continue'
    $listOutput = & $ctestExe --test-dir $enhancedBuild -C RelWithDebInfo -N -R $testFilter 2>&1 |
        Tee-Object -FilePath $listLog
    $listCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($listCode -ne 0) { throw "Gate 3 test discovery failed with exit code $listCode" }
    $countMatch = [regex]::Match(($listOutput -join "`n"), 'Total Tests:\s+(\d+)')
    if (-not $countMatch.Success -or [int]$countMatch.Groups[1].Value -ne 7) {
        throw "Gate 3 expected exactly 7 architecture-invariant tests"
    }

    Write-Host '=== Gate 3: run architecture-invariant tests only ==='
    $testLog = Join-Path $logs 'gate3-tests.log'
    $testXml = Join-Path $logs 'gate3-invariants.xml'
    $ErrorActionPreference = 'Continue'
    & $ctestExe --test-dir $enhancedBuild -C RelWithDebInfo --output-on-failure --output-junit $testXml -R $testFilter 2>&1 |
        Tee-Object -FilePath $testLog
    $testCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($testCode -ne 0) { throw "Gate 3 architecture-invariant tests failed with exit code $testCode" }

    $evidence = Join-Path $logs 'gate3-architecture-invariants.txt'
    @(
        'Gate 3 architecture-invariant tests: PASS',
        "Pinned upstream: $actualRoot",
        'Configuration: Visual Studio 2022 x64 RelWithDebInfo',
        "CTest filter: $testFilter",
        'Test groups discovered and passed: 7',
        'Full validation suite: not run',
        'Simulation/GUI/app target: not run',
        'Runtime packaging: not run'
    ) | Set-Content -Path $evidence -Encoding utf8

    Write-Host '=== GATE 3 ARCHITECTURE INVARIANTS PASSED ==='
    exit 0
}

if ($env:GENERIC_RUNTIME_SMOKE_ONLY -eq '1') {
    Write-Host '=== Gate 4: configure generic runtime smoke tests ==='
    $configureLog = Join-Path $logs 'gate4-configure.log'
    $previousErrorActionPreference = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    & $cmakeExe -S $source -B $enhancedBuild @commonConfigure 2>&1 |
        Tee-Object -FilePath $configureLog
    $configureCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($configureCode -ne 0) { throw "Gate 4 configure failed with exit code $configureCode" }

    Write-Host '=== Gate 4: build unit-test target only ==='
    $buildLog = Join-Path $logs 'gate4-build.log'
    $ErrorActionPreference = 'Continue'
    & $cmakeExe --build $enhancedBuild --config RelWithDebInfo --target engine-sim-test --parallel 2>&1 |
        Tee-Object -FilePath $buildLog
    $buildCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($buildCode -ne 0) { throw "Gate 4 test build failed with exit code $buildCode" }

    $testBinary = Join-Path $enhancedBuild 'RelWithDebInfo\engine-sim-test.exe'
    if (-not (Test-Path $testBinary)) { throw "Gate 4 expected test binary missing: $testBinary" }

    $testFilter = 'ForcedInductionRuntimeSmoke'
    $listLog = Join-Path $logs 'gate4-test-list.log'
    $ErrorActionPreference = 'Continue'
    $listOutput = & $ctestExe --test-dir $enhancedBuild -C RelWithDebInfo -N -R $testFilter 2>&1 |
        Tee-Object -FilePath $listLog
    $listCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($listCode -ne 0) { throw "Gate 4 test discovery failed with exit code $listCode" }
    $countMatch = [regex]::Match(($listOutput -join "`n"), 'Total Tests:\s+(\d+)')
    if (-not $countMatch.Success -or [int]$countMatch.Groups[1].Value -ne 5) {
        throw "Gate 4 expected exactly 5 generic runtime smoke tests"
    }

    Write-Host '=== Gate 4: run generic runtime smoke tests only ==='
    $testLog = Join-Path $logs 'gate4-tests.log'
    $testXml = Join-Path $logs 'gate4-runtime-smoke.xml'
    $ErrorActionPreference = 'Continue'
    & $ctestExe --test-dir $enhancedBuild -C RelWithDebInfo --output-on-failure --output-junit $testXml -R $testFilter 2>&1 |
        Tee-Object -FilePath $testLog
    $testCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($testCode -ne 0) { throw "Gate 4 generic runtime smoke tests failed with exit code $testCode" }

    $evidence = Join-Path $logs 'gate4-generic-runtime-smoke.txt'
    @(
        'Gate 4 generic runtime smoke tests: PASS',
        "Pinned upstream: $actualRoot",
        'Configuration: Visual Studio 2022 x64 RelWithDebInfo',
        "CTest filter: $testFilter",
        'Synthetic scenarios discovered and passed: 5',
        'ALCO calibration/runtime: not run',
        'Full validation suite: not run',
        'GUI/app target and runtime packaging: not run'
    ) | Set-Content -Path $evidence -Encoding utf8

    Write-Host '=== GATE 4 GENERIC RUNTIME SMOKE PASSED ==='
    exit 0
}

if ($env:ALCO_INTEGRATION_ONLY -eq '1') {
    Write-Host '=== Gate 5: install ALCO validation configuration ==='
    New-Item -ItemType Directory -Force -Path (Join-Path $source 'assets\engines\alco') | Out-Null
    Copy-Item -Force (Join-Path $overlay 'assets\engines\alco\alco_251d_diesel_turbo.mr') (Join-Path $source 'assets\engines\alco\alco_251d_diesel_turbo.mr')
    Copy-Item -Force (Join-Path $overlay 'assets\alco_main.mr') (Join-Path $source 'assets\alco_main.mr')

    Write-Host '=== Gate 5: configure ALCO integration validation ==='
    $configureLog = Join-Path $logs 'gate5-configure.log'
    $previousErrorActionPreference = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    & $cmakeExe -S $source -B $enhancedBuild @commonConfigure 2>&1 |
        Tee-Object -FilePath $configureLog
    $configureCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($configureCode -ne 0) { throw "Gate 5 configure failed with exit code $configureCode" }

    Write-Host '=== Gate 5: build integration target only ==='
    $buildLog = Join-Path $logs 'gate5-build.log'
    $ErrorActionPreference = 'Continue'
    & $cmakeExe --build $enhancedBuild --config RelWithDebInfo --target engine-sim-integration-validation --parallel 2>&1 |
        Tee-Object -FilePath $buildLog
    $buildCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($buildCode -ne 0) { throw "Gate 5 integration build failed with exit code $buildCode" }

    $testBinary = Join-Path $enhancedBuild 'RelWithDebInfo\engine-sim-integration-validation.exe'
    if (-not (Test-Path $testBinary)) { throw "Gate 5 expected test binary missing: $testBinary" }

    $testFilter = '^AlcoIntegrationValidation\.'
    $listLog = Join-Path $logs 'gate5-test-list.log'
    $ErrorActionPreference = 'Continue'
    $listOutput = & $ctestExe --test-dir $enhancedBuild -C RelWithDebInfo -N -R $testFilter 2>&1 |
        Tee-Object -FilePath $listLog
    $listCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($listCode -ne 0) { throw "Gate 5 test discovery failed with exit code $listCode" }
    $countMatch = [regex]::Match(($listOutput -join "`n"), 'Total Tests:\s+(\d+)')
    if (-not $countMatch.Success -or [int]$countMatch.Groups[1].Value -ne 7) {
        throw "Gate 5 expected exactly 7 isolated integration tests"
    }

    Write-Host '=== Gate 5: run ALCO integration validation only ==='
    $testLog = Join-Path $logs 'gate5-tests.log'
    $testXml = Join-Path $logs 'gate5-alco-integration.xml'
    $ErrorActionPreference = 'Continue'
    & $ctestExe --test-dir $enhancedBuild -C RelWithDebInfo --output-on-failure --output-junit $testXml -R $testFilter 2>&1 |
        Tee-Object -FilePath $testLog
    $testCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($testCode -ne 0) { throw "Gate 5 ALCO integration validation failed with exit code $testCode" }

    $evidence = Join-Path $logs 'gate5-alco-integration.txt'
    @(
        'Gate 5 ALCO integration validation: PASS',
        "Pinned upstream: $actualRoot",
        'Configuration: Visual Studio 2022 x64 RelWithDebInfo',
        'Null model: pinned upstream assets/main.mr (Subaru EJ25 SI)',
        'Full model: assets/alco_main.mr (ALCO 6-251D CI + fixed-geometry turbo + aftercooler)',
        "CTest filter: $testFilter",
        'Isolated integration cases discovered and passed: 7',
        'Boost/power calibration: not performed',
        'Full validation suite: not run',
        'GUI/app target and runtime packaging: not run'
    ) | Set-Content -Path $evidence -Encoding utf8

    Write-Host '=== GATE 5 ALCO INTEGRATION VALIDATION PASSED ==='
    exit 0
}

if ($env:ALCO_251B_LOADED_TRANSIENT_ONLY -eq '1') {
    Write-Host '=== Gate 6: install exact official v0.1.14a SI null reference ==='
    $releaseUrl = 'https://github.com/Engine-Simulator/engine-sim-community-edition/releases/download/v0.1.14a/engine-sim-v0.1.14a.zip'
    $releaseZip = Join-Path $work 'engine-sim-v0.1.14a.zip'
    $releaseExtract = Join-Path $work 'engine-sim-v0.1.14a-release'
    Invoke-WebRequest -Uri $releaseUrl -OutFile $releaseZip
    $releaseHash = (Get-FileHash $releaseZip -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($releaseHash -ne '2fc1e7c2ad6a94af4bbe78e69907b57aad3d5ebda7d55563e24fa2e14707fbe6') {
        throw "official v0.1.14a archive SHA256 mismatch: $releaseHash"
    }
    Expand-Archive -Force $releaseZip $releaseExtract
    $stockSiSource = Join-Path $releaseExtract 'engine-sim-v0.1.14a\assets\engines\kohler\kohler_ch750.mr'
    if (-not (Test-Path $stockSiSource)) { throw "official v0.1.14a SI reference missing: $stockSiSource" }
    $stockSiHash = (Get-FileHash $stockSiSource -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($stockSiHash -ne 'c729091fe9c6d3ad3761564850bfad437ea441fee7308a7e7c7ea226ad8b2196') {
        throw "official v0.1.14a Kohler reference SHA256 mismatch: $stockSiHash"
    }
    Copy-Item -Force $stockSiSource (Join-Path $source 'assets\v014a_reference_kohler.mr')

    Write-Host '=== Gate 6: configure v0.1.14a compatibility and native 16-251B validation ==='
    $configureLog = Join-Path $logs 'gate6-configure.log'
    $previousErrorActionPreference = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    & $cmakeExe -S $source -B $enhancedBuild @commonConfigure 2>&1 |
        Tee-Object -FilePath $configureLog
    $configureCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($configureCode -ne 0) { throw "Gate 6 configure failed with exit code $configureCode" }

    Write-Host '=== Gate 6: build loaded-transient target only ==='
    $buildLog = Join-Path $logs 'gate6-build.log'
    $ErrorActionPreference = 'Continue'
    & $cmakeExe --build $enhancedBuild --config RelWithDebInfo --target engine-sim-loaded-transient-validation --parallel 2>&1 |
        Tee-Object -FilePath $buildLog
    $buildCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($buildCode -ne 0) { throw "Gate 6 loaded-transient build failed with exit code $buildCode" }

    $testBinary = Join-Path $enhancedBuild 'RelWithDebInfo\engine-sim-loaded-transient-validation.exe'
    if (-not (Test-Path $testBinary)) { throw "Gate 6 expected test binary missing: $testBinary" }

    $compatFilter = '^V014aCompatibility\.'
    $compatListLog = Join-Path $logs 'gate6-v014a-test-list.log'
    $ErrorActionPreference = 'Continue'
    $compatListOutput = & $ctestExe --test-dir $enhancedBuild -C RelWithDebInfo -N -R $compatFilter 2>&1 |
        Tee-Object -FilePath $compatListLog
    $compatListCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($compatListCode -ne 0) { throw "Gate 6 v0.1.14a test discovery failed with exit code $compatListCode" }
    $compatCountMatch = [regex]::Match(($compatListOutput -join "`n"), 'Total Tests:\s+(\d+)')
    if (-not $compatCountMatch.Success -or [int]$compatCountMatch.Groups[1].Value -ne 1) {
        throw "Gate 6 expected exactly 1 official v0.1.14a SI compatibility test"
    }

    Write-Host '=== Gate 6: run official v0.1.14a stock SI compatibility first ==='
    $compatLog = Join-Path $logs 'gate6-v014a-tests.log'
    $compatXml = Join-Path $logs 'gate6-v014a-stock-si.xml'
    $ErrorActionPreference = 'Continue'
    & $ctestExe --test-dir $enhancedBuild -C RelWithDebInfo --output-on-failure --output-junit $compatXml -R $compatFilter 2>&1 |
        Tee-Object -FilePath $compatLog
    $compatCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($compatCode -ne 0) { throw "Gate 6 official v0.1.14a SI compatibility failed with exit code $compatCode" }

    $testFilter = '^Alco251BLoadedTransient\.'
    $listLog = Join-Path $logs 'gate6-test-list.log'
    $ErrorActionPreference = 'Continue'
    $listOutput = & $ctestExe --test-dir $enhancedBuild -C RelWithDebInfo -N -R $testFilter 2>&1 |
        Tee-Object -FilePath $listLog
    $listCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($listCode -ne 0) { throw "Gate 6 test discovery failed with exit code $listCode" }
    $countMatch = [regex]::Match(($listOutput -join "`n"), 'Total Tests:\s+(\d+)')
    if (-not $countMatch.Success -or [int]$countMatch.Groups[1].Value -ne 2) {
        throw "Gate 6 expected exactly 2 loaded-transient tests"
    }

    Write-Host '=== Gate 6: run native 16-251B loaded-transient validation only ==='
    $testLog = Join-Path $logs 'gate6-tests.log'
    $testXml = Join-Path $logs 'gate6-alco-251b-loaded-transient.xml'
    $ErrorActionPreference = 'Continue'
    & $ctestExe --test-dir $enhancedBuild -C RelWithDebInfo --output-on-failure --output-junit $testXml -R $testFilter 2>&1 |
        Tee-Object -FilePath $testLog
    $testCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorActionPreference
    if ($testCode -ne 0) { throw "Gate 6 native 16-251B loaded-transient validation failed with exit code $testCode" }

    $evidence = Join-Path $logs 'gate6-alco-251b-loaded-transient.txt'
    @(
        'Gate 6 native 16-251B loaded-transient validation: PASS',
        "Pinned upstream: $actualRoot",
        "Official v0.1.14a archive SHA256: $releaseHash",
        "Official Kohler CH750 SHA256: $stockSiHash",
        'Configuration: Visual Studio 2022 x64 RelWithDebInfo',
        'Null reference: unmodified official v0.1.14a assets/engines/kohler/kohler_ch750.mr',
        'Compatibility result: stock naturally aspirated SI script loaded with forced induction disabled and original exhaust routing',
        'Reference: assets/alco_16_251b_main.mr (native CI, governor, four-scroll fixed Model 710, aftercooler)',
        "CTest filter: $testFilter",
        'Loaded-transient cases discovered and passed: 2',
        'Assertions: causal response and stable release; no absolute boost or power target',
        'Boost/power calibration: not performed',
        'Full validation suite: not run',
        'GUI/app target and runtime packaging: not run'
    ) | Set-Content -Path $evidence -Encoding utf8

    Write-Host '=== GATE 6 NATIVE 16-251B LOADED TRANSIENT PASSED ==='
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
        "if ($env:CORE_COMPILE_ONLY -ne '1' -and $env:ARCHITECTURE_TESTS_ONLY -ne '1' -and $env:GENERIC_RUNTIME_SMOKE_ONLY -ne '1' -and $env:ALCO_INTEGRATION_ONLY -ne '1' -and $env:ALCO_251B_LOADED_TRANSIENT_ONLY -ne '1')",
        "if ($env:CORE_COMPILE_ONLY -eq '1')",
        "if ($env:ARCHITECTURE_TESTS_ONLY -eq '1')",
        "if ($env:GENERIC_RUNTIME_SMOKE_ONLY -eq '1')",
        "if ($env:ALCO_INTEGRATION_ONLY -eq '1')",
        "if ($env:ALCO_251B_LOADED_TRANSIENT_ONLY -eq '1')",
        "'engine-sim-script-interpreter'",
        "'engine-sim-runtime-smoke'",
        "$ErrorActionPreference = 'Continue'",
        "$ErrorActionPreference = $previousErrorActionPreference",
        "Gate 2 core compile and link: PASS",
        "GATE 2 CORE COMPILE AND LINK PASSED",
        "Gate 3 expected exactly 7 architecture-invariant tests",
        "Gate 3 architecture-invariant tests: PASS",
        "GATE 3 ARCHITECTURE INVARIANTS PASSED",
        "Gate 4 expected exactly 5 generic runtime smoke tests",
        "Gate 4 generic runtime smoke tests: PASS",
        "GATE 4 GENERIC RUNTIME SMOKE PASSED",
        "Gate 5 expected exactly 7 isolated integration tests",
        "Gate 5 ALCO integration validation: PASS",
        "GATE 5 ALCO INTEGRATION VALIDATION PASSED",
        "Gate 6 expected exactly 2 loaded-transient tests",
        "Gate 6 expected exactly 1 official v0.1.14a SI compatibility test",
        "Gate 6 native 16-251B loaded-transient validation: PASS",
        "GATE 6 NATIVE 16-251B LOADED TRANSIENT PASSED",
    )
    for token in required:
        if token not in text:
            raise RuntimeError(f"post-condition failed: missing {token!r}")

    end_anchor = "New-Item -ItemType Directory -Force -Path (Join-Path $source 'assets\\engines\\alco')"

    start = text.index("if ($env:CORE_COMPILE_ONLY -eq '1')")
    end = text.find("if ($env:ARCHITECTURE_TESTS_ONLY -eq '1')", start)
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

    start = text.index("if ($env:ARCHITECTURE_TESTS_ONLY -eq '1')")
    end = text.find("if ($env:GENERIC_RUNTIME_SMOKE_ONLY -eq '1')", start)
    if end < 0:
        raise RuntimeError("post-condition failed: could not delimit Gate 3 branch")
    gate3 = text[start:end]
    required_gate3 = (
        "--target engine-sim-test",
        "-N -R $testFilter",
        "--output-junit $testXml -R $testFilter",
        "[int]$countMatch.Groups[1].Value -ne 7",
    )
    for token in required_gate3:
        if token not in gate3:
            raise RuntimeError(f"post-condition failed: Gate 3 lacks required action {token!r}")
    forbidden_gate3 = (
        "engine-sim-app",
        "engine-sim-script-smoke.exe' stock",
        "engine-sim-runtime-smoke.exe' stock",
        "package_runtime.ps1",
        "Compress-Archive -Path (Join-Path $runtime",
    )
    for token in forbidden_gate3:
        if token in gate3:
            raise RuntimeError(f"post-condition failed: Gate 3 contains forbidden action {token!r}")

    start = text.index("if ($env:GENERIC_RUNTIME_SMOKE_ONLY -eq '1')")
    end = text.find("if ($env:ALCO_INTEGRATION_ONLY -eq '1')", start)
    if end < 0:
        raise RuntimeError("post-condition failed: could not delimit Gate 4 branch")
    gate4 = text[start:end]
    required_gate4 = (
        "--target engine-sim-test",
        "-N -R $testFilter",
        "--output-junit $testXml -R $testFilter",
        "[int]$countMatch.Groups[1].Value -ne 5",
        "'ALCO calibration/runtime: not run'",
    )
    for token in required_gate4:
        if token not in gate4:
            raise RuntimeError(f"post-condition failed: Gate 4 lacks required action {token!r}")
    forbidden_gate4 = (
        "engine-sim-app",
        "engine-sim-script-smoke.exe' stock",
        "engine-sim-runtime-smoke.exe' stock",
        "assets/alco_main.mr",
        "package_runtime.ps1",
        "Compress-Archive -Path (Join-Path $runtime",
    )
    for token in forbidden_gate4:
        if token in gate4:
            raise RuntimeError(f"post-condition failed: Gate 4 contains forbidden action {token!r}")

    start = text.index("if ($env:ALCO_INTEGRATION_ONLY -eq '1')")
    end = text.find("if ($env:ALCO_251B_LOADED_TRANSIENT_ONLY -eq '1')", start)
    if end < 0:
        raise RuntimeError("post-condition failed: could not delimit Gate 5 branch")
    gate5 = text[start:end]
    required_gate5 = (
        "--target engine-sim-integration-validation",
        "assets/main.mr (Subaru EJ25 SI)",
        "assets\\alco_main.mr",
        "-N -R $testFilter",
        "--output-junit $testXml -R $testFilter",
        "[int]$countMatch.Groups[1].Value -ne 7",
        "'Boost/power calibration: not performed'",
    )
    for token in required_gate5:
        if token not in gate5:
            raise RuntimeError(f"post-condition failed: Gate 5 lacks required action {token!r}")
    forbidden_gate5 = (
        "engine-sim-app",
        "engine-sim-script-smoke.exe' stock",
        "package_runtime.ps1",
        "Compress-Archive -Path (Join-Path $runtime",
    )
    for token in forbidden_gate5:
        if token in gate5:
            raise RuntimeError(f"post-condition failed: Gate 5 contains forbidden action {token!r}")

    start = text.index("if ($env:ALCO_251B_LOADED_TRANSIENT_ONLY -eq '1')")
    end = text.find("\n" + end_anchor, start)
    if end < 0:
        raise RuntimeError("post-condition failed: could not delimit Gate 6 branch")
    gate6 = text[start:end]
    required_gate6 = (
        "--target engine-sim-loaded-transient-validation",
        "engine-sim-v0.1.14a.zip",
        "2fc1e7c2ad6a94af4bbe78e69907b57aad3d5ebda7d55563e24fa2e14707fbe6",
        "c729091fe9c6d3ad3761564850bfad437ea441fee7308a7e7c7ea226ad8b2196",
        "assets\\v014a_reference_kohler.mr",
        "[int]$compatCountMatch.Groups[1].Value -ne 1",
        "--output-junit $compatXml -R $compatFilter",
        "assets/alco_16_251b_main.mr",
        "-N -R $testFilter",
        "--output-junit $testXml -R $testFilter",
        "[int]$countMatch.Groups[1].Value -ne 2",
        "'Boost/power calibration: not performed'",
        "'Full validation suite: not run'",
    )
    for token in required_gate6:
        if token not in gate6:
            raise RuntimeError(f"post-condition failed: Gate 6 lacks required action {token!r}")
    forbidden_gate6 = (
        "engine-sim-app",
        "engine-sim-script-smoke.exe' stock",
        "package_runtime.ps1",
        "Compress-Archive -Path (Join-Path $runtime",
    )
    for token in forbidden_gate6:
        if token in gate6:
            raise RuntimeError(f"post-condition failed: Gate 6 contains forbidden action {token!r}")


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
        text, SOURCE_CAPTURE_TAIL, GATED_BLOCKS, "source-capture tail"
    )
    verify_postconditions(text)
    target.write_text(text, encoding="utf-8", newline="\n")
    print("Gate 2/3/4/5/6 scoped CI paths enabled; post-conditions PASS")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except RuntimeError as exc:
        print(f"validation gate CI adapter failed: {exc}", file=sys.stderr)
        raise SystemExit(1)
