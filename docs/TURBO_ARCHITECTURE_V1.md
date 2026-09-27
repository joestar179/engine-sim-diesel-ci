# Generic Forced-Induction / Turbocharged Gas-Path Architecture V1 — LOCKED

Status: **LOCKED FOR IMPLEMENTATION**

This document is the implementation contract for generic turbocharged-engine support in Engine Simulator. The ALCO 251-D is one validation case only. The core architecture must also support future automotive petrol and diesel engines without engine-specific C++ changes.

Changes to this topology require a deliberate architecture revision. Calibration changes must not change the flow graph.

## 1. Design scope

The forced-induction core is **combustion-system agnostic**.

It must support, through script configuration:
- spark-ignition and compression-ignition engines;
- port-injected, direct-injected, carbureted, or other existing/future fuel systems;
- throttled and unthrottled air paths;
- fixed-geometry and variable-geometry turbines;
- wastegated and non-wastegated turbines;
- single, parallel, twin-scroll and future sequential turbo arrangements;
- charge-air cooling or no charge-air cooling;
- compressor bypass / blow-off valves where required;
- naturally aspirated engines with all new systems disabled.

Turbocharging must not be implemented as:
- an imposed intake-pressure multiplier;
- an exhaust-pressure observer;
- a power multiplier;
- a fuel-system surrogate;
- an engine-specific ALCO special case.

The turbo subsystem owns **gas flow, thermodynamics and turbo shaft dynamics**. It exposes air/exhaust state to the engine's existing combustion and control systems; it does not dictate how fuel or ignition are implemented.

## 2. Reusable component graph

The architecture is a graph of reusable gas volumes, flow restrictions, rotating machines and optional control valves.

A turbo installation consists of one or more **Turbo Groups**. Each Turbo Group owns one turbo shaft and connects one or more exhaust runner groups to one or more intake/charge paths.

The initial implementation may instantiate one Turbo Group, but no core data structure may assume that an engine can only ever have one.

Examples enabled by the graph:

```text
Single turbo:
all cylinders → one turbine → one shaft → one compressor

Parallel twin turbo:
bank A → turbine A → shaft A → compressor A ┐
bank B → turbine B → shaft B → compressor B ├→ shared or separate charge plenums

Twin-scroll:
runner group A → turbine inlet channel A ┐
runner group B → turbine inlet channel B ├→ one turbine rotor / one shaft
                                     (pulse separation retained upstream)

Sequential:
runner/manifold routing + control valves → turbo A and/or turbo B
```

## 3. Locked air-side topology

Canonical turbocharged path:

```text
Ambient reservoir
    ↓
optional inlet/filter restriction
    ↓
COMPRESSOR
    ↓
compressor-discharge gas volume
    ↓
optional intercooler / aftercooler
    ↓
optional throttle / air-control valve
    ↓
charge-air plenum / intake manifold
    ↓
existing intake runner system
    ↓
existing intake-valve flow
    ↓
Cylinder
```

The throttle / air-control element is optional and engine-defined:
- conventional SI engines may use it for load control;
- many diesels omit it during normal operation;
- future engines may use an air valve for shutdown, EGR management or other purposes.

The forced-induction core must not reinterpret throttle position as diesel fueling.

For naturally aspirated engines, the original Engine Simulator atmosphere → intake path remains unchanged.

### Compressor requirements

The compressor must transfer **actual gas mass**. Charge pressure must emerge from:
- compressor delivery;
- charge-system volume;
- restrictions;
- cooler losses;
- cylinder consumption.

Charge pressure must never be written directly as a target supply pressure.

At zero turbo speed:
- the compressor cannot create pressure rise;
- the installation must still permit configurable passive/naturally aspirated airflow for cranking and low-speed operation.

Required compressor behavior:
- corrected-flow / capacity approximation or map;
- pressure-ratio capability versus corrected speed and flow;
- efficiency behavior;
- surge boundary or stable reduced-order surrogate;
- choke/maximum-flow boundary;
- compressor torque/power debited from its actual shaft;
- discharge temperature from compressor work and efficiency.

## 4. Fuel and combustion-system boundary

**Fuel delivery is not part of the turbo topology.**

The forced-induction subsystem exposes physical state such as:
- manifold pressure and temperature;
- charge-air mass flow;
- trapped air / oxygen estimate;
- compressor state;
- exhaust state.

The engine's configured control/combustion system decides how those states influence fuel.

Examples:

```text
Mechanical diesel:
operator command → governor → rack → optional smoke/air limiter → direct injector

Electronic diesel:
pedal/load request → ECU torque request → air/smoke/EGT limiters → direct injector

Port-injected petrol:
pedal → throttle / ECU → fuel metering in intake/port → spark control

Direct-injected petrol:
pedal → throttle / ECU → DI fuel command → spark control
```

A generic air/fuel or smoke limiter interface may consume forced-induction state, but it is optional and must not be hard-wired into the turbo model.

Turbocharging never assumes that fuel is injected into either the intake or the cylinder.

## 5. Locked exhaust-side topology

Canonical turbocharged path:

```text
Cylinder
    ↓
existing exhaust-valve / port flow
    ↓
existing per-cylinder head runner + primary
    ↓
configured runner group / pulse group
    ↓
dedicated pre-turbine manifold or scroll GasSystem
    ↓
turbine swallowing-capacity / nozzle restriction
    ↓
TURBINE ROTOR energy extraction
    ↓
post-turbine exhaust GasSystem
    ↓
existing downstream exhaust restriction / silencer equivalent
    ↓
Atmosphere
```

The original `ExhaustSystem::m_system` remains **downstream of the turbine** for turbocharged configurations. It must not be repurposed as the turbine-inlet manifold.

The dedicated pre-turbine manifold represents only real pre-turbine plumbing:
- manifold/header volume;
- turbine scroll/inlet volume where represented;
- pulse-group separation where configured.

For turbo-disabled engines, the original runner → ExhaustSystem path is retained exactly.

## 6. Turbo shaft abstraction

Each Turbo Group owns one shaft:

```text
turbine torque
- compressor torque
- bearing / windage / friction torque
= shaft inertia × angular acceleration
```

A shaft owns:
- angular speed;
- inertia;
- friction/drag;
- maximum allowed speed;
- attached turbine(s), normally one in V1;
- attached compressor(s), normally one in V1.

The data model must use references/indices rather than a single engine-global turbo assumption so future compound or multi-turbo layouts do not require an architectural rewrite.

Turbine and compressor power are evaluated from the same shaft state during each fluid substep.

## 7. Turbine model

The turbine is simultaneously:
1. a gas-flow restriction;
2. an energy extractor;
3. a torque source for its shaft.

Required inputs:
- turbine-inlet gas state;
- turbine-outlet pressure/state;
- shaft speed;
- swallowing-capacity / effective-nozzle behavior;
- turbine efficiency behavior;
- optional geometry/control state.

The same gas transfer that determines turbine mass flow must determine:
- turbine pressure ratio;
- gas enthalpy drop;
- turbine shaft power;
- post-turbine gas state.

Energy is removed from the gas crossing the turbine boundary, not later from an unrelated reservoir.

At zero shaft speed, exhaust may still pass through the turbine and produce starting torque.

### Fixed geometry and VGT/VNT

The core exposes turbine effective flow capacity as a physical parameter.

For a fixed-geometry turbine it is fixed/map-derived.

For VGT/VNT it may be varied by a generic actuator/controller:
```text
control command → vane position → effective swallowing capacity / efficiency behavior
```

VGT is optional and disabled unless configured.

## 8. Charge-air cooling

Charge-air cooling is a reusable optional component operating on actual compressor-discharge gas.

Minimum V1 behavior:
- configurable effectiveness;
- heat removal from real flowing/stored charge gas;
- no mass creation/destruction;
- optional configurable pressure loss;
- no arbitrary pressure multiplication.

The core names this generically as charge-air cooling; engine definitions may call the physical device an intercooler or aftercooler.

## 9. Optional flow-control branches

All are generic and **disabled by default**.

### Wastegate

```text
pre-turbine manifold
    ├── turbine → post-turbine exhaust
    └── wastegate bypass → post-turbine exhaust
```

Configurable:
- enabled;
- actuator/control command;
- effective flow capacity/area;
- response rate / time constant;
- optional control source (boost, exhaust pressure, ECU/controller command).

### Compressor bypass / blow-off / recirculation valve

```text
compressor discharge / charge plenum
    → bypass valve
    → compressor inlet or atmosphere
```

Configurable:
- enabled;
- recirculating or vent-to-atmosphere destination;
- opening criterion/command;
- effective flow capacity;
- response rate.

This is useful for throttled petrol engines and other surge-sensitive layouts, but is not assumed for diesels.

### Future routing/control valves

The graph must permit additional flow branches without rewriting the turbine/compressor models, including:
- sequential-turbo routing valves;
- charge-path bypasses;
- exhaust brake / backpressure devices;
- EGR branches.

These are not required for V1 implementation unless configured, but the graph must not preclude them.

## 10. Geometry and state ownership

The following physical states are distinct and must not be silently aliased:

1. cylinder gas;
2. per-cylinder exhaust runner/primary;
3. each configured pre-turbine manifold/scroll;
4. each turbine outlet/post-turbine exhaust volume;
5. exhaust ambient reservoir;
6. compressor inlet/ambient state;
7. each compressor-discharge volume;
8. each charge-air cooling stage;
9. each charge plenum/intake manifold;
10. existing intake runner;
11. cylinder intake charge.

Script-exposed geometry/parameters should include where practical:
- runner-to-turbo grouping;
- pre-turbine manifold/scroll volume and characteristic area;
- turbine swallowing-capacity/effective-area parameters or map;
- compressor inlet restriction;
- compressor characteristics/map approximation;
- compressor-discharge volume;
- charge-plenum volume;
- cooler effectiveness and pressure loss;
- throttle/air-valve placement/control where applicable;
- wastegate properties;
- compressor-bypass properties;
- VGT actuator properties;
- shaft inertia/friction/max speed.

Unknown dimensions remain calibration values and must be labelled as such.

## 11. Controller boundary

Turbo physics and boost control are separate.

The physical model exposes states and actuator inputs.

Possible controllers include:
- passive pressure-actuated wastegate;
- electronic boost controller;
- VGT controller;
- mechanical diesel governor plus smoke limiter;
- petrol ECU using throttle, wastegate, fueling and ignition;
- no controller for a free-floating fixed-geometry turbo.

No controller may bypass the physical flow/shaft model by directly assigning boost pressure or turbo speed.

Dyno RPM hold remains external test-bench behavior and is unrelated to engine boost/governor control.

## 12. Backward compatibility

When forced induction is disabled:
- cylinder → runner → existing ExhaustSystem path is exactly the original Engine Simulator path;
- atmosphere → existing Intake path is exactly the original path;
- existing SI throttle/fueling behavior is preserved;
- no turbo gas volumes influence the engine;
- optional valves/actuators are inactive;
- existing scripts parse unchanged.

New parameters default to disabled/pass-through behavior.

## 13. Numerical rules

- No pressure, temperature, airflow, torque or power state is imposed merely to hit a target output.
- Flow remains bounded near zero pressure difference and through reverse-flow conditions.
- Flow devices cannot transfer more mass than source systems contain.
- Turbines cannot extract more energy than is available in the transferred gas.
- Compressors debit real shaft energy and cannot create pressure rise from a stationary shaft.
- Shaft speed is bounded by configured maximum speed.
- Passive cranking/low-RPM airflow remains possible.
- Optional closed valves do not leak unless configured.
- Multi-turbo groups are updated deterministically within each fluid substep.

## 14. Required telemetry

At minimum, per relevant group/device:
- cylinder pressure/temperature at exhaust-valve opening;
- per-cylinder runner peak pressure/temperature;
- pre-turbine manifold/scroll pressure and temperature;
- turbine mass flow;
- turbine pressure ratio;
- turbine power;
- wastegate flow where enabled;
- post-turbine pressure/temperature;
- turbo shaft speed;
- compressor mass flow;
- compressor pressure ratio;
- compressor power;
- compressor-discharge temperature;
- cooler inlet/outlet temperature and pressure;
- charge-plenum pressure/temperature;
- bypass-valve flow where enabled;
- cylinder/intake air flow or trapped-air estimate.

Fuel telemetry belongs to the engine control/combustion layer but should be logged alongside turbo state for validation.

## 15. Validation matrix

Implementation is not accepted until all applicable gates pass.

### Generic/core gates
1. Build succeeds.
2. Existing upstream regression comparison passes.
3. Turbo-disabled SI path remains unchanged.
4. Zero exhaust energy produces no shaft acceleration/boost.
5. Exhaust blowdown reaches the correct configured pre-turbine group.
6. Turbine restriction creates causal pre/post pressure difference.
7. Turbine power accelerates the correct shaft.
8. Compressor consumes shaft power and transfers actual air mass.
9. Charge pressure emerges from mass balance, not a forced boundary.
10. Wastegate disabled path is inert; enabled path bypasses real exhaust mass.
11. Compressor bypass disabled path is inert; enabled path transfers real charge mass.
12. VGT disabled/fixed path is unchanged; variable geometry changes swallowing capacity causally.
13. Multi-group routing keeps gas and shaft states isolated except at explicitly shared plenums.
14. Cranking and low-RPM passive airflow remain stable.

### Petrol validation case
A representative turbo SI engine must eventually demonstrate:
- existing throttle/load behavior preserved;
- compressor response to throttle transients;
- optional bypass/BOV preventing or reducing modeled surge where configured;
- wastegate/VGT boost control without direct boost assignment.

### Diesel validation case
A representative turbo diesel must eventually demonstrate:
- unthrottled/passive cranking airflow;
- fuel control separate from charge-air path;
- air/smoke limiting able to consume actual trapped-air/boost state;
- stable governor/boost interaction.

### Loaded transient gate
Increasing engine load/fuel/airflow must produce a physically consistent chain:
```text
combustion/exhaust energy
→ pre-turbine state
→ turbine mass flow and power
→ shaft acceleration
→ compressor mass flow/work
→ charge state
→ changed cylinder air mass
```

Detailed calibration is not allowed until this chain is stable.

## 16. Initial ALCO 251-D validation configuration

The ALCO 251-D is **only the first validation engine**.

Initial configuration:
- compression ignition/direct injection: enabled by the engine's combustion layer;
- fixed-geometry turbo group: enabled;
- charge-air aftercooler: enabled;
- conventional throttle in charge path: disabled;
- wastegate: disabled unless documentation supports one;
- compressor bypass/BOV: disabled unless documentation supports one;
- VGT: disabled;
- pre-turbine geometry: calibration/estimated until sourced;
- turbine/compressor map parameters: calibration/estimated until sourced.

No documented ALCO physical specification may be altered solely to force target power, boost, RPM or sound.

---

**Architecture freeze rule:** implementation patches may change equations, data structures, numerical stabilization and script exposure as required, but they may not change the component ordering, ownership boundaries, multi-group capability or combustion-system independence above without revising this document first.
