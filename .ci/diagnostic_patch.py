from pathlib import Path
root=Path('.')
p=root/'tools/apply_ce_enhancement.py'
s=p.read_text(encoding='utf-8')
def rep(a,b):
 global s
 if a not in s: raise SystemExit('diagnostic patch anchor missing: '+a[:80])
 s=s.replace(a,b,1)
rep('        void recordDirectInjectedFuelMass(double mass) { m_directInjectedFuelMass += mass; }\\n',
    '        void recordDirectInjectedFuelMass(double mass) { m_directInjectedFuelMass += mass; }\\n        void recordDirectBurnedFuelMass(double mass) { m_directBurnedFuelMass += mass; }\\n        double getDirectInjectedFuelMass() const { return m_directInjectedFuelMass; }\\n        double getDirectBurnedFuelMass() const { return m_directBurnedFuelMass; }\\n        void recordCompressionIgnitionConditions(double temperature, double pressure) { if (temperature > m_maxCiTemperature) m_maxCiTemperature = temperature; if (pressure > m_maxCiPressure) m_maxCiPressure = pressure; }\\n        double getMaxCompressionIgnitionTemperature() const { return m_maxCiTemperature; }\\n        double getMaxCompressionIgnitionPressure() const { return m_maxCiPressure; }\\n')
rep('        double m_directInjectedFuelMass = 0.0;\\n\\n        Throttle *m_throttle;',
    '        double m_directInjectedFuelMass = 0.0;\\n        double m_directBurnedFuelMass = 0.0;\\n        double m_maxCiTemperature = 0.0;\\n        double m_maxCiPressure = 0.0;\\n\\n        Throttle *m_throttle;')
s=s.replace('    m_directInjectedFuelMass = 0.0;\\n', '    m_directInjectedFuelMass = 0.0;\\n    m_directBurnedFuelMass = 0.0;\\n    m_maxCiTemperature = 0.0;\\n    m_maxCiPressure = 0.0;\\n')
rep('    if (!m_engine->isCompressionIgnition()) return;\n    const auto result = m_engine->getCompressionIgnitionModel()->step(',
    '    if (!m_engine->isCompressionIgnition()) return;\n    m_engine->recordCompressionIgnitionConditions(m_system.temperature(), m_system.pressure());\n    const auto result = m_engine->getCompressionIgnitionModel()->step(')
rep('        m_nBurntFuel += mass;\n        if (dt > 0.0) {',
    '        m_nBurntFuel += mass;\n        m_engine->recordDirectBurnedFuelMass(mass);\n        if (dt > 0.0) {')
# Temporary architecture audit: expose the generated turbo implementation in CI logs.
for needle in ('void Engine::updateTurbo', 'm_turbocharger.step', 'TurbochargerModel::step'):
    positions=[]
    start=0
    while True:
        pos=s.find(needle,start)
        if pos < 0: break
        positions.append(pos); start=pos+1
    for pos in positions:
        print('--- TURBO SOURCE CONTEXT:', needle, pos, '---')
        print(s[max(0,pos-5000):pos+12000].replace('\\\\n','\\n'))

p.write_text(s,encoding='utf-8')
p=root/'test/runtime_engine_smoke.cpp'
s=p.read_text(encoding='utf-8')
run_anchor='    constexpr int frames = 180; // 3 seconds at 60 Hz\n'
if run_anchor not in s: raise SystemExit('runtime loop anchor missing')
s=s.replace(run_anchor, '    engine->getIgnitionModule()->m_enabled = true;\n' + run_anchor, 1)
a='        << " | max_turbo_rpm=" << maxTurboRpm\n        << "\\n";'
b='        << " | max_turbo_rpm=" << maxTurboRpm\n        << " | injected_fuel_g=" << engine->getDirectInjectedFuelMass() * 1000.0\n        << " | burned_fuel_g=" << engine->getDirectBurnedFuelMass() * 1000.0\n        << " | max_ci_temp_K=" << engine->getMaxCompressionIgnitionTemperature()\n        << " | max_ci_pressure_MPa=" << engine->getMaxCompressionIgnitionPressure() / 1.0e6\n        << " | final_compressor_pr=" << engine->getCompressorPressureRatio()\n        << " | final_compressor_power_W=" << engine->getCompressorPower()\n        << " | final_exhaust_pressure_kPa=" << engine->getExhaustSystem(0)->getSystem()->pressure() / 1000.0\n        << " | final_exhaust_temp_K=" << engine->getExhaustSystem(0)->getSystem()->temperature()\n        << "\\n";'
if a not in s: raise SystemExit('runtime telemetry anchor missing')
p.write_text(s.replace(a,b,1),encoding='utf-8')
print('CI combustion diagnostics applied')

exec(compile((root/'.ci/turbo_arch_patch.py').read_text(encoding='utf-8'), '.ci/turbo_arch_patch.py', 'exec'))

# Normalize the pinned Delta Studio D3DX linkage for distributable RelWithDebInfo builds.
p=root/'tools/windows_ci.ps1'
s=p.read_text(encoding='utf-8')
anchor='if ($actualDelta -ne $deltaPin) { throw "wrong Delta commit: $actualDelta" }\n'
fix=r'''if ($actualDelta -ne $deltaPin) { throw "wrong Delta commit: $actualDelta" }

# Delta Studio b7d0a046 links retail and debug D3DX libraries unconditionally.
# RelWithDebInfo must use only the redistributable retail imports.
$deltaCmake = Join-Path $source 'dependencies\\submodules\\delta-studio\\CMakeLists.txt'
$deltaText = [IO.File]::ReadAllText($deltaCmake)
foreach ($debugImport in @(
    '    ${D3DX_LIBS}/d3dx9d.lib',
    '    ${D3DX_LIBS}/d3dx10d.lib',
    '    ${D3DX_LIBS}/d3dx11d.lib'
)) {
    if (-not $deltaText.Contains($debugImport)) { throw "Expected pinned Delta D3DX debug import missing: $debugImport" }
    $deltaText = $deltaText.Replace("$debugImport`r`n", '').Replace("$debugImport`n", '')
}
[IO.File]::WriteAllText($deltaCmake, $deltaText, (New-Object System.Text.UTF8Encoding($false)))
Write-Host 'Normalized pinned Delta D3DX linkage to retail imports only.'
'''
if anchor not in s: raise SystemExit('windows_ci Delta pin anchor missing')
s=s.replace(anchor,fix,1)
p.write_text(s,encoding='utf-8')
print('CI packaging linkage fix applied')


# Seed restore overwrites tools/windows_ci.ps1. Re-insert the full topology
# application step here, after restore, so the generated source definitely
# receives the in-series compressor/turbine gas-path patch.
p=root/'tools/windows_ci.ps1'
s=p.read_text(encoding='utf-8')
anchor="""python (Join-Path $overlay 'tools\\apply_ce_enhancement.py') $source
if ($LASTEXITCODE -ne 0) { throw 'enhancement patch failed' }
"""
insert=anchor+"""python (Join-Path $overlay '.ci\\full_turbo_topology_patch.py') $source
if ($LASTEXITCODE -ne 0) { throw 'full turbo topology patch failed' }
python (Join-Path $overlay '.ci\\restore_engine_metrics_patch.py') $source
if ($LASTEXITCODE -ne 0) { throw 'Engine metrics restoration patch failed' }
"""
if anchor not in s: raise SystemExit('windows_ci enhancement-application anchor missing')
if "full_turbo_topology_patch.py" not in s:
    s=s.replace(anchor,insert,1)
p.write_text(s,encoding='utf-8')
print('CI full in-series turbo topology hook applied after seed restore')
