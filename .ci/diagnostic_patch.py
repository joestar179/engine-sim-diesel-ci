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
