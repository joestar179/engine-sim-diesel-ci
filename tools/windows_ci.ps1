$ErrorActionPreference = 'Stop'

$overlay = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$work = Join-Path $env:GITHUB_WORKSPACE '_ci'
$source = Join-Path $work 'engine-sim'
$baselineBuild = Join-Path $work 'build-baseline'
$enhancedBuild = Join-Path $work 'build-enhanced'
$runtime = Join-Path $work 'runtime'
$logs = Join-Path $work 'logs'
New-Item -ItemType Directory -Force -Path $work,$logs | Out-Null

# Preserve the reconstructed overlay for full-source audit and deterministic repair.
# This is diagnostic evidence only; it does not alter the source being built.
$auditZip = Join-Path $logs 'reconstructed-overlay.zip'
if (Test-Path $auditZip) { Remove-Item -Force $auditZip }
$overlayAuditItems = Get-ChildItem -Force $overlay | Where-Object { $_.Name -notin @('.git','_ci') }
Compress-Archive -Path $overlayAuditItems.FullName -DestinationPath $auditZip -CompressionLevel Optimal

$rootPin = '56725cc012581282567900b15871018d55b7ab42'
$piranhaPin = '432f0b122bb1663b686c553c7e7269300afac3bc'
$deltaPin = 'b7d0a046733b924d12706baf1e5e59ba427aa7b1'

Write-Host '=== Install deterministic build prerequisites ==='
choco install winflexbison3 --no-progress -y
if ($LASTEXITCODE -ne 0) { throw 'winflexbison3 installation failed' }

$cmakeVersion = '3.31.12'
$cmakeZip = Join-Path $work "cmake-$cmakeVersion.zip"
$cmakeRoot = Join-Path $work "cmake-$cmakeVersion-windows-x86_64"
$cmakeExe = Join-Path $cmakeRoot 'bin\cmake.exe'
$ctestExe = Join-Path $cmakeRoot 'bin\ctest.exe'
if (-not (Test-Path $cmakeExe)) {
    curl.exe -L --fail --retry 4 -o $cmakeZip "https://github.com/Kitware/CMake/releases/download/v$cmakeVersion/cmake-$cmakeVersion-windows-x86_64.zip"
    if ($LASTEXITCODE -ne 0) { throw 'CMake download failed' }
    Expand-Archive -Force $cmakeZip $work
}

$boostRoot = 'C:\local\boost_1_78_0'
$boostLib = Join-Path $boostRoot 'lib64-msvc-14.3'
$boostHeader = Join-Path $boostRoot 'boost\version.hpp'
if (-not (Test-Path $boostHeader)) {
    $boostExe = Join-Path $work 'boost_1_78_0-msvc-14.3-64.exe'
    $boostUrl = 'https://archives.boost.io/release/1.78.0/binaries/boost_1_78_0-msvc-14.3-64.exe'
    $boostSha = 'b8911c98c2a95faa516aca354872b0ea63962a437382822aaf7bca95f528db65'
    curl.exe -L --fail --retry 4 -o $boostExe $boostUrl
    if ($LASTEXITCODE -ne 0) { throw 'Boost 1.78 download failed' }
    $actual = (Get-FileHash -Algorithm SHA256 $boostExe).Hash.ToLowerInvariant()
    if ($actual -ne $boostSha) { throw "Boost SHA256 mismatch: $actual" }
    $p = Start-Process -FilePath $boostExe -ArgumentList '/VERYSILENT','/SUPPRESSMSGBOXES','/NORESTART' -Wait -PassThru
    if ($p.ExitCode -ne 0) { throw "Boost installer exited $($p.ExitCode)" }
}
if (-not (Test-Path $boostHeader)) { throw 'Boost 1.78 headers missing after install' }

$vcpkg = $env:VCPKG_INSTALLATION_ROOT
if (-not $vcpkg) { $vcpkg = 'C:\vcpkg' }
$vcpkgExe = Join-Path $vcpkg 'vcpkg.exe'
if (-not (Test-Path $vcpkgExe)) { throw "vcpkg not found: $vcpkgExe" }
& $vcpkgExe install sdl2:x64-windows sdl2-image:x64-windows
if ($LASTEXITCODE -ne 0) { throw 'SDL2 vcpkg install failed' }
$sdlRoot = Join-Path $vcpkg 'installed\x64-windows'
$env:SDL2DIR = $sdlRoot
$env:SDL2IMAGEDIR = $sdlRoot
$env:BOOST_ROOT = $boostRoot
$env:BOOST_LIBRARYDIR = $boostLib

Write-Host '=== Clone exact classic source ==='
git clone --recurse-submodules https://github.com/ange-yaghi/engine-sim.git $source
if ($LASTEXITCODE -ne 0) { throw 'upstream clone failed' }
git -C $source checkout $rootPin
if ($LASTEXITCODE -ne 0) { throw 'root pin checkout failed' }
git -C $source submodule update --init --recursive
if ($LASTEXITCODE -ne 0) { throw 'submodule checkout failed' }

$actualRoot = (git -C $source rev-parse HEAD).Trim()
$actualPiranha = (git -C (Join-Path $source 'dependencies\submodules\piranha') rev-parse HEAD).Trim()
$actualDelta = (git -C (Join-Path $source 'dependencies\submodules\delta-studio') rev-parse HEAD).Trim()
if ($actualRoot -ne $rootPin) { throw "wrong root commit: $actualRoot" }
if ($actualPiranha -ne $piranhaPin) { throw "wrong Piranha commit: $actualPiranha" }
if ($actualDelta -ne $deltaPin) { throw "wrong Delta commit: $actualDelta" }

$commonConfigure = @(
    '-G','Visual Studio 17 2022','-A','x64',
    '-DDISCORD_ENABLED:BOOL=OFF','-DDTV:BOOL=OFF','-DPIRANHA_ENABLED:BOOL=ON',
    "-DBOOST_ROOT:PATH=$boostRoot","-DBOOST_LIBRARYDIR:PATH=$boostLib",
    "-DBoost_INCLUDE_DIR:PATH=$boostRoot",'-DBoost_NO_SYSTEM_PATHS:BOOL=ON',
    '-DBoost_NO_BOOST_CMAKE:BOOL=ON','-DBoost_USE_STATIC_LIBS:BOOL=ON',
    '-DBoost_USE_MULTITHREADED:BOOL=ON','-DBoost_USE_STATIC_RUNTIME:BOOL=OFF'
)

Write-Host '=== Establish pristine upstream test baseline ==='
& $cmakeExe -S $source -B $baselineBuild @commonConfigure
if ($LASTEXITCODE -ne 0) { throw 'baseline configure failed' }
& $cmakeExe --build $baselineBuild --config RelWithDebInfo --target engine-sim-test --parallel
if ($LASTEXITCODE -ne 0) { throw 'baseline test build failed' }
$baselineXml = Join-Path $logs 'baseline.xml'
& $ctestExe --test-dir $baselineBuild -C RelWithDebInfo --output-on-failure --output-junit $baselineXml
Write-Host "Pristine baseline CTest exit code: $LASTEXITCODE (failures are compared, not blindly rejected)"

Write-Host '=== Static audit overlay before application ==='
python (Join-Path $overlay 'tools\static_audit.py') $overlay
if ($LASTEXITCODE -ne 0) { throw 'overlay static audit failed' }

Write-Host '=== Apply one clean enhancement patch ==='
python (Join-Path $overlay 'tools\apply_ce_enhancement.py') $source
if ($LASTEXITCODE -ne 0) { throw 'enhancement patch failed' }
New-Item -ItemType Directory -Force -Path (Join-Path $source 'assets\engines\alco') | Out-Null
Copy-Item -Force (Join-Path $overlay 'assets\engines\alco\alco_251d_diesel_turbo.mr') (Join-Path $source 'assets\engines\alco\alco_251d_diesel_turbo.mr')
Copy-Item -Force (Join-Path $overlay 'assets\alco_main.mr') (Join-Path $source 'assets\alco_main.mr')

Write-Host '=== Configure/build enhanced source ==='
& $cmakeExe -S $source -B $enhancedBuild @commonConfigure
if ($LASTEXITCODE -ne 0) { throw 'enhanced configure failed' }
& $cmakeExe --build $enhancedBuild --config RelWithDebInfo --target engine-sim-test engine-sim-script-smoke engine-sim-runtime-smoke engine-sim-app --parallel
if ($LASTEXITCODE -ne 0) { throw 'enhanced build failed' }

$enhancedXml = Join-Path $logs 'enhanced.xml'
& $ctestExe --test-dir $enhancedBuild -C RelWithDebInfo --output-on-failure --output-junit $enhancedXml
Write-Host "Enhanced CTest exit code: $LASTEXITCODE"
python (Join-Path $overlay 'tools\compare_ctest_results.py') $baselineXml $enhancedXml
if ($LASTEXITCODE -ne 0) { throw 'enhancement introduced a C++ regression' }

Write-Host '=== Execute stock and ALCO scripts semantically ==='
$scriptSmoke = Join-Path $enhancedBuild 'RelWithDebInfo\engine-sim-script-smoke.exe'
Push-Location $source
try {
    & $scriptSmoke 'assets/main.mr' stock
    if ($LASTEXITCODE -ne 0) { throw 'stock semantic script smoke failed' }
    & $scriptSmoke 'assets/alco_main.mr' alco
    if ($LASTEXITCODE -ne 0) { throw 'ALCO semantic script smoke failed' }
} finally { Pop-Location }

Write-Host '=== Run coupled ALCO starter/governor/CI/turbo smoke ==='
$runtimeSmoke = Join-Path $enhancedBuild 'RelWithDebInfo\engine-sim-runtime-smoke.exe'
Push-Location $source
try {
    & $runtimeSmoke 'assets/alco_main.mr'
    if ($LASTEXITCODE -ne 0) { throw 'coupled ALCO runtime smoke failed' }
} finally { Pop-Location }

Write-Host '=== Build source-coherent runtime package ==='
& (Join-Path $overlay 'tools\package_runtime.ps1') -Source $source -Build $enhancedBuild -Out $runtime -VcpkgRoot $vcpkg
if ($LASTEXITCODE -ne 0) { throw 'runtime packaging failed' }

$zip = Join-Path $work 'engine-sim-diesel-windows-x64.zip'
if (Test-Path $zip) { Remove-Item -Force $zip }
Compress-Archive -Path (Join-Path $runtime '*') -DestinationPath $zip -CompressionLevel Optimal
Write-Host '=== WINDOWS CI PASSED ==='
