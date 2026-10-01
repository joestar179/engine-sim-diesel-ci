# P3 — 2014 Mazda 2.0L SKYACTIV-G EPA/NVFEL

**Status: ACCEPT — validation-grade for defined claims**

## Supported validation claims
- steady-state indicated-to-brake thermodynamic response using measured fuel mass/boundary/control states
- brake torque/power over withheld speed-load cells
- fuel efficiency / BSFC / BTE surface
- wide-open-throttle curve
- hot motoring/friction-envelope check
- cross-fuel knock/control response between the two EPA fuels

## Explicitly excluded claims
- production-engine rotational inertia transient validation
- injector spray/plume CFD validation
- acoustic spectral validation

## Configuration
- **displacement_cm3:** 1998
- **bore_mm:** 83.5
- **stroke_mm:** 91.2
- **compression_ratio:** 13.0
- **aspiration:** naturally aspirated
- **fuel_system:** gasoline direct injection
- **rated_power_kW_rpm:** [115, 6000]
- **rated_torque_Nm_rpm:** [203, 4000]
- **peak_rail_pressure_bar:** 200
- **intake_cam_phaser:** 75 crank-deg advance from base IVC 58 deg BTDC
- **exhaust_cam_phaser:** 45 crank-deg retard from base EVC 14 deg ATDC
- **ecu:** stock production ECU, engine tethered to donor vehicle

## Complete map
- Speed nodes: 18
- Torque nodes: 24
- Fuel cells: 432
- Fuel units: g/s
- Source fuel LHV: 42.887 MJ/kg

## Validation split
- **calibration:** Use physical geometry plus measured fuel mass and measured/logged boundary/control state. If combustion-shape parameters need tuning, use only a declared subset of steady-state cells.
- **holdout_A:** Entire WOT curve except any explicitly used calibration point.
- **holdout_B:** Contiguous block of speed/load cells not used for combustion calibration.
- **holdout_C:** Second-fuel map/response after parameters are frozen on Tier-2 fuel.
- **holdout_D:** Motoring/friction curve if friction model was calibrated elsewhere.

## Remaining U outside claimed validation domain
- exact injector nozzle-hole geometry/spray targeting — not validation-critical for lumped combustion if measured fuel mass is supplied
- production crank/flywheel/accessory inertia — validation-critical only for transient RPM response; do not claim transient validation from this pack
- instrumented exact-engine sound recording