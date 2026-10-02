# Generic Forced-Induction V1 implementation record

This document maps the readable implementation in
`patches/forced_induction_v1/` to the locked contract in
`docs/TURBO_ARCHITECTURE_V1.md`.

## Implemented physical topology

Air, per `TurboGroup`:

```text
ambient reservoir
  -> inlet/filter restriction
  -> compressor (real bounded gas transfer; shaft work debited)
  -> compressor-discharge GasSystem
  -> optional charge-air cooler GasSystem and pressure-loss restrictions
  -> optional throttle/air valve
  -> charge-plenum GasSystem
  -> existing Intake::m_system
  -> existing per-cylinder intake runner and valve
  -> cylinder
```

Exhaust, per configured route:

```text
cylinder
  -> existing exhaust valve
  -> existing per-cylinder runner/primary
  -> TurboGroup pre-turbine scroll GasSystem
  -> turbine swallowing restriction and boundary energy extraction
  -> original ExhaustSystem::m_system (post-turbine)
  -> original outlet restriction
  -> atmosphere
```

Shaft, per `TurboGroup`:

```text
turbine torque - compressor torque - bearing/friction torque
  -> shaft angular acceleration, inertia and maximum-speed bound
```

## Ownership and compatibility

- `ForcedInductionSystem` owns a deterministic vector of `TurboGroup` objects.
- Each group owns one shaft, compressor-side volumes, one or more pulse-separated
  pre-turbine scroll volumes, optional-device actuator state and telemetry.
- Exhaust and intake routing uses indices; engine-global single-turbo state is
  not used.
- Multiple groups may feed an explicitly shared downstream intake volume.
- A runner cannot feed two groups without a future explicit routing valve.
- With no enabled groups, no forced-induction gas volume is connected and the
  original intake and exhaust paths are retained.
- The v0.1.14a `backflow_atmospheric_mixing` exhaust input is retained. Its
  default of zero preserves the pinned classic exhaust boundary; nonzero values
  alter the composition only of real pressure-driven atmospheric backflow.
- The forced-induction core contains no diesel, governor, rack, injection,
  ignition or ALCO-specific logic.

## Optional devices

The V1 physical branches are implemented and disabled by default:

- fixed geometry or VGT/VNT effective turbine capacity;
- wastegate transfer from pre-turbine scroll to post-turbine exhaust;
- compressor bypass to inlet or blow-off to ambient;
- charge-air cooling with actual stored/flowing-gas energy removal;
- charge-air pressure loss through flow restrictions;
- charge-path throttle/air valve using the existing engine control position.

Initial actuator commands and response time constants are script parameters.
Runtime command setters on each `TurboGroup` expose wastegate, compressor-bypass
and VGT actuation to generic controllers. Closed-loop ECU, pneumatic wastegate
and VGT control policies remain outside the physical component, as required by
the controller boundary.

## Telemetry

Per group telemetry includes pre-turbine pressure/temperature by scroll,
turbine flow/pressure ratio/power, wastegate flow, post-turbine state, shaft
speed, compressor flow/pressure ratio/power/discharge temperature, cooler
inlet/outlet state, charge-plenum state, bypass flow and delivered intake flow.

Per-cylinder telemetry includes pressure and temperature at exhaust-valve
opening, runner peak pressure/temperature and trapped-air moles.

## Deliberately not implemented in V1

These do not require a gas-path or shaft rewrite:

- closed-loop boost, wastegate or VGT controllers;
- sequential-turbo routing-valve control;
- measured compressor/turbine maps (the current model is a bounded reduced-order
  approximation);
- a native multi-`TurboGroup` `.mr` construction node. The C++ data model and
  routing are multi-group; the backward-compatible `.mr` surface currently
  creates one group and keeps ALCO values in its engine definition.

No performance calibration or ALCO boost/power tuning is part of this commit.
