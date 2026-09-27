# Turbocharged Gas-Path Architecture V1 — LOCKED

Status: **LOCKED FOR IMPLEMENTATION**

This document is the implementation contract for generic turbocharged-engine support. Changes to this topology require a deliberate architecture revision; calibration changes must not change the flow graph.

## 1. Design objective

Turbocharging must be a real, in-series thermodynamic subsystem. It must not be implemented as:
- an imposed intake-pressure multiplier;
- an exhaust-pressure observer;
- a power multiplier;
- an engine-specific ALCO special case.

All new behavior is disabled by default so existing spark-ignition engines retain their original path and behavior.

## 2. Locked gas-path topology

### Air side

```text
Ambient reservoir
    ↓
Compressor inlet restriction / filter
    ↓
COMPRESSOR
    ↓
Compressor-discharge gas volume
    ↓
AFTERCOOLER / intercooler heat removal
    ↓
Charge-air plenum / intake manifold
    ↓
Existing intake runner system
    ↓
Existing intake-valve flow
    ↓
Cylinder
```

For naturally aspirated engines, the original Engine Simulator atmosphere → intake path remains unchanged.

The compressor must transfer actual gas mass. Charge pressure must emerge from compressor flow into the charge volume minus cylinder consumption; it must not be written directly as a supply pressure.

At zero turbo speed, the compressor cannot create pressure rise. A configurable passive-flow path must permit cranking / naturally aspirated airflow through the compressor installation.

### Fuel side

```text
Operator speed/load command
    ↓
Governor / rack command
    ↓
Air / smoke / boost fuel limiter
    ↓
Direct injector
    ↓
Cylinder
```

Fuel remains separate from the air path until injection into the cylinder. Turbocharging never injects fuel into the intake.

### Exhaust side

```text
Cylinder
    ↓
Existing exhaust-valve / port flow
    ↓
Existing per-cylinder head runner + primary
    ↓
NEW shared pre-turbine manifold GasSystem
    ↓
Turbine nozzle / swallowing-capacity restriction
    ↓
TURBINE ROTOR energy extraction
    ↓
Existing ExhaustSystem gas volume (post-turbine exhaust)
    ↓
Existing outlet restriction / silencer equivalent
    ↓
Atmosphere
```

The original `ExhaustSystem::m_system` is **post-turbine**. It must never be repurposed as the entire turbine-inlet manifold.

The new pre-turbine manifold represents only the actual manifold/header volume joining the primaries to the turbine.

## 3. Turbo shaft

One shaft couples turbine and compressor:

```text
turbine torque
- compressor torque
- shaft/bearing friction
= shaft inertia × angular acceleration
```

The shaft owns:
- angular speed;
- inertia;
- friction/drag;
- maximum allowed speed.

Turbine and compressor power are computed from the same shaft state during each fluid substep.

## 4. Turbine model

The turbine is both:
1. a gas-flow restriction; and
2. an energy extractor.

Required inputs:
- pre-turbine pressure and temperature;
- post-turbine pressure;
- shaft speed;
- turbine swallowing-capacity / effective-nozzle parameter;
- turbine efficiency behavior.

The same gas transfer that determines turbine mass flow must determine:
- pressure ratio;
- enthalpy drop;
- turbine shaft power;
- post-turbine gas state.

Energy must be removed from gas crossing the turbine boundary, not later from an arbitrary exhaust reservoir.

At zero shaft speed, exhaust may still pass through the turbine and produce starting torque.

## 5. Compressor model

The compressor is both:
1. an air-flow device; and
2. a shaft-power consumer.

Required behavior:
- corrected mass-flow / capacity approximation;
- pressure-ratio capability as a function of shaft speed and flow;
- efficiency approximation;
- choke boundary;
- surge boundary or stable reduced-order surrogate;
- zero pressure rise at zero shaft speed;
- compressor torque debited from the same shaft.

Compressor discharge temperature follows compressor work and efficiency.

## 6. Charge-air cooling

The aftercooler operates on actual compressor-discharge gas.

Minimum V1 model:
- configurable effectiveness;
- heat removal from compressor-discharge/charge gas;
- no mass creation/destruction;
- no arbitrary pressure multiplication.

Pressure loss may be added as a generic configurable restriction, defaulting to zero.

## 7. Optional bypass / control devices

These are generic capabilities and are **disabled by default**.

### Wastegate

```text
pre-turbine manifold
    ├── turbine → post-turbine exhaust
    └── wastegate bypass → post-turbine exhaust
```

Required configurable properties:
- enabled;
- control/opening command;
- effective flow capacity;
- actuator response time.

No wastegate is assumed for a specific engine unless its definition enables one.

### Compressor bypass / blow-off valve

```text
compressor discharge / charge plenum
    → optional bypass
    → compressor inlet / atmosphere
```

Required configurable properties:
- enabled;
- opening criterion / command;
- effective flow capacity;
- actuator response.

Disabled for engines that do not use one.

## 8. Fuel-air limiting

Governor-requested fuel is not automatically guaranteed.

Actual permitted fuel may be limited by generic configurable constraints:
- available trapped air / oxygen;
- smoke-equivalence-ratio limit;
- boost / manifold pressure;
- engine speed;
- optional exhaust-temperature protection.

These limits are separate from the governor.

## 9. State ownership and geometry

The following states are distinct and must not be aliased:

1. cylinder gas;
2. per-cylinder exhaust runner/primary;
3. shared pre-turbine manifold;
4. post-turbine ExhaustSystem;
5. ambient exhaust reservoir;
6. compressor inlet / ambient;
7. compressor discharge;
8. charge-air plenum;
9. existing intake runner;
10. cylinder intake charge.

Geometry exposed through normal engine definitions where practical:
- pre-turbine manifold volume;
- pre-turbine manifold characteristic area;
- turbine swallowing capacity / effective area;
- compressor inlet restriction;
- compressor discharge volume;
- charge-plenum volume;
- aftercooler effectiveness / optional pressure loss;
- optional wastegate capacity;
- optional compressor-bypass capacity.

Unknown dimensions are calibration values and must be labelled as such.

## 10. Backward compatibility

When turbocharging is disabled:
- cylinder → runner → existing ExhaustSystem path is exactly the original Engine Simulator path;
- atmosphere → existing Intake path is exactly the original path;
- no additional turbo gas volumes influence SI engines;
- optional valves are inactive;
- existing scripts parse unchanged.

## 11. Numerical rules

- No pressure or temperature state may be imposed merely to force target boost.
- Flow must remain bounded at zero/near-zero pressure difference.
- Turbine and compressor cannot transfer more mass than their source systems contain.
- Turbine cannot extract more gas energy than is physically available.
- Compressor cannot consume more shaft energy than the shaft/turbine can supply during the substep.
- Shaft speed is bounded by configured maximum speed.
- Reverse flow must remain numerically stable.
- Cranking and low-RPM airflow must remain possible.

## 12. Required telemetry

At minimum:
- cylinder pressure/temperature at exhaust-valve opening;
- per-cylinder runner peak pressure/temperature;
- pre-turbine manifold pressure/temperature;
- turbine mass flow;
- turbine pressure ratio;
- turbine power;
- post-turbine pressure/temperature;
- turbo shaft speed;
- compressor mass flow;
- compressor pressure ratio;
- compressor power;
- compressor-discharge temperature;
- charge-plenum pressure/temperature;
- cylinder/intake air flow or trapped air estimate;
- requested fuel and limited/actual injected fuel.

## 13. Validation gates

Implementation is not accepted until all applicable gates pass:

1. Build succeeds.
2. Existing baseline regression comparison passes.
3. Turbo-disabled SI path remains unchanged.
4. Turbo-enabled engine starts and idles without artificial boost.
5. Exhaust blowdown reaches the runner and dedicated pre-turbine manifold.
6. Turbine restriction creates physically causal pre/post pressure difference.
7. Turbine power accelerates the shared shaft.
8. Compressor consumes shaft power and transfers real air mass.
9. Charge pressure emerges from mass balance, not a forced boundary.
10. Increasing load/fuel increases exhaust energy, turbine power, shaft speed and charge flow in a physically consistent direction.
11. Wastegate/bypass disabled paths have no effect.
12. Longer loaded/notch validation is stable before detailed calibration.

## 14. ALCO 251-D validation configuration

The ALCO 251-D is a validation case, not an architectural dependency.

For the initial ALCO configuration:
- direct diesel injection: enabled;
- fixed-geometry turbo path: enabled;
- aftercooler: enabled;
- wastegate: disabled unless documentation supports one;
- compressor bypass/blow-off valve: disabled unless documentation supports one;
- pre-turbine manifold dimensions: calibration/estimated until sourced;
- turbine/compressor map parameters: calibration/estimated until sourced.

No documented ALCO physical specification may be altered solely to force target power, boost, RPM, or sound.

---

**Architecture freeze rule:** implementation patches may change equations, data structures, numerical stabilization and script exposure as required, but they may not change the gas-path ordering above without revising this document first.
