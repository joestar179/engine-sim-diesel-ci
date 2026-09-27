from pathlib import Path
import sys

if len(sys.argv) != 2:
    raise SystemExit('usage: restore_engine_metrics_patch.py <engine-sim-source>')
root = Path(sys.argv[1])
p = root / 'src/engine.cpp'
s = p.read_text(encoding='utf-8')

# The full turbo-topology transform rewrites engine.cpp and accidentally removed
# these long-standing Engine API definitions. Restore them without touching the
# new gas-path topology.
if 'double Engine::getManifoldPressure() const' in s:
    raise SystemExit('getManifoldPressure already defined; refusing duplicate repair')

anchor = 'double Engine::getIntakeFlowRate() const {'
pos = s.find(anchor)
if pos < 0:
    raise SystemExit('engine metrics repair anchor missing')

brace = s.find('{', pos)
depth = 0
end = None
for i in range(brace, len(s)):
    if s[i] == '{':
        depth += 1
    elif s[i] == '}':
        depth -= 1
        if depth == 0:
            end = i + 1
            break
if end is None:
    raise SystemExit('could not locate end of getIntakeFlowRate')

block = r'''

// Restored core Engine telemetry API after turbo-topology rewrite.
// These report the actual simulated manifold/exhaust GasSystem states; they do
// not alter intake, combustion, turbine, compressor, or governor behaviour.
double Engine::getManifoldPressure() const {
    if (m_intakeCount <= 0) return units::pressure(1.0, units::atm);

    double pressureSum = 0.0;
    for (int i = 0; i < m_intakeCount; ++i) {
        pressureSum += m_intakes[i].m_system.pressure();
    }

    return pressureSum / m_intakeCount;
}

double Engine::getIntakeAfr() const {
    double totalOxygen = 0.0;
    double totalFuel = 0.0;
    for (int i = 0; i < m_intakeCount; ++i) {
        totalOxygen += m_intakes[i].m_system.n_o2();
        totalFuel += m_intakes[i].m_system.n_fuel();
    }

    constexpr double octaneMolarMass = units::mass(114.23, units::g);
    constexpr double oxygenMolarMass = units::mass(31.9988, units::g);

    if (totalFuel == 0) return 0;
    return (oxygenMolarMass * totalOxygen / 0.21) / (totalFuel * octaneMolarMass);
}

double Engine::getExhaustO2() const {
    double totalInert = 0.0;
    double totalOxygen = 0.0;
    double totalFuel = 0.0;
    for (int i = 0; i < m_exhaustSystemCount; ++i) {
        totalInert += m_exhaustSystems[i].m_system.n_inert();
        totalOxygen += m_exhaustSystems[i].m_system.n_o2();
        totalFuel += m_exhaustSystems[i].m_system.n_fuel();
    }

    constexpr double octaneMolarMass = units::mass(114.23, units::g);
    constexpr double oxygenMolarMass = units::mass(31.9988, units::g);
    constexpr double nitrogenMolarMass = units::mass(28.014, units::g);

    if (totalFuel == 0) return 0;
    return (oxygenMolarMass * totalOxygen) /
        (totalFuel * octaneMolarMass + nitrogenMolarMass * totalInert + oxygenMolarMass * totalOxygen);
}
'''

s = s[:end] + block + s[end:]
p.write_text(s, encoding='utf-8')
print('restored Engine manifold/AFR/exhaust-O2 telemetry API')
