"""Layer-2 equipment defaults: size- and technology-continuous rules of thumb.

Every default returns (value, low, high, source). Sources:
  HEYWOOD  Heywood, Internal Combustion Engine Fundamentals (textbook typical
           values; chapter given; values recalled, to be page-verified)
  SANDOVAL Sandoval & Heywood, MIT 2003 (PNH friction constants, table 4.2)
  PROJECT  bounded by this project's documented engines (GX390 F, Mazda F/D,
           Kohler F, Deere D) — range widened to cover them
  C        engineering estimate; plausible range stated
Defaults are fixed in advance and never adjusted to a target (CLAUDE.md 0e).
"""
import math

def d(value, low, high, source):
    return dict(value=value, low=low, high=high, source=source)

# --- valvetrain ------------------------------------------------------------
def intake_valve_diameter(bore_mm, valves_per_cyl):
    # 2-valve: Heywood ch. 6 ~0.42-0.46 B; GX390 0.375 B (F) -> range widened.
    # 4-valve: Heywood ~0.35-0.37 B; Mazda 0.398 B (F).
    if valves_per_cyl >= 4:
        return d(0.37 * bore_mm, 0.33 * bore_mm, 0.41 * bore_mm, 'HEYWOOD ch6 / PROJECT Mazda')
    return d(0.42 * bore_mm, 0.36 * bore_mm, 0.46 * bore_mm, 'HEYWOOD ch6 / PROJECT GX390')

def exhaust_valve_diameter(intake_d_mm, valves_per_cyl):
    # Exhaust/intake head ratio: GX390 31/33 (F), Mazda 28/33.2 (F); typical 0.80-0.87.
    return d(0.84 * intake_d_mm, 0.78 * intake_d_mm, 0.94 * intake_d_mm, 'HEYWOOD ch6 / PROJECT')

def valve_lift(valve_d_mm):
    # L/D ~0.25 (curtain area = throat area); GX390 0.25 (F), Mazda 0.29 (R).
    return d(0.27 * valve_d_mm, 0.22 * valve_d_mm, 0.31 * valve_d_mm, 'HEYWOOD ch6 / PROJECT')

def cam_class(rated_rpm):
    """Duration at 0.050 in (deg) and lobe centres (deg) by rated-speed class."""
    # GX390 (3600 rpm): 212 / 200 (F); Mazda (6000 rpm): 228 / 211 (C); racing > 7500: 250+.
    t = min(1.0, max(0.0, (rated_rpm - 3600.0) / (6000.0 - 3600.0)))
    intake = 205 + t * (225 - 205)
    exhaust = 200 + t * (215 - 200)
    return dict(intake_duration=d(intake, intake - 15, intake + 20, 'C / PROJECT GX390, Mazda'),
                exhaust_duration=d(exhaust, exhaust - 15, exhaust + 20, 'C / PROJECT'),
                intake_center=d(110.0, 100.0, 120.0, 'HEYWOOD ch6 (typical IVO ~10 BTDC, IVC ~50 ABDC)'),
                exhaust_center=d(110.0, 100.0, 120.0, 'HEYWOOD ch6'))

def valvetrain_friction(valvetrain):
    """Sandoval table 4.2 constants (flat, roller, osc. hydrodynamic, osc. mixed)."""
    table = {
        'ohv':              (400.0, 0.0, 0.5, 32.1),
        'sohc_finger':      (600.0, 0.0, 0.2, 42.8),
        'sohc_rocker':      (400.0, 0.0, 0.5, 21.4),
        'sohc_direct':      (200.0, 0.0, 0.5, 10.7),
        'dohc_finger':      (600.0, 0.0, 0.2, 25.8),
        'dohc_direct':      (133.0, 0.0, 0.5, 10.7),
        'dohc_roller_finger': (0.0, 0.0227, 0.2, 25.8),
    }
    return table[valvetrain], 'SANDOVAL table 4.2'

# --- bottom end ------------------------------------------------------------
def rod_length(stroke_mm):
    # Rod/stroke 1.5-1.9; Mazda 1.70 (F), GX390 1.75 (F).
    return d(1.70 * stroke_mm, 1.5 * stroke_mm, 1.9 * stroke_mm, 'HEYWOOD ch2 / PROJECT')

def bearings(bore_mm):
    # Journal diameter / bore: Mazda 0.56 (F), GX390 crankpin 0.41 (F); width ~0.38 D.
    jd = 0.56 * bore_mm
    return dict(main_d=d(jd, 0.40 * bore_mm, 0.70 * bore_mm, 'PROJECT'),
                main_l=d(0.38 * jd, 0.30 * jd, 0.50 * jd, 'PROJECT Mazda 18/47'),
                rod_d=d(jd, 0.40 * bore_mm, 0.70 * bore_mm, 'PROJECT'),
                rod_l=d(0.38 * jd, 0.30 * jd, 0.50 * jd, 'C'))

# --- breathing -------------------------------------------------------------
def rated_airflow_gs(displacement_l, rated_rpm, ve=0.85, rho=1.18):
    return ve * displacement_l * rated_rpm / 120.0 * rho      # g/s (4-stroke)

def gs_to_cfm(gs, rho=1.18):
    return gs / rho / 0.471947                                # L/s -> CFM

def intake_restriction_cfm(rated_air_cfm):
    # Throttle/carb k_carb rating ~ 1.8-3 x rated airflow (GX390 2.9, Mazda 1.9).
    return d(2.4 * rated_air_cfm, 1.8 * rated_air_cfm, 3.0 * rated_air_cfm, 'PROJECT')

def exhaust_outlet_cfm(rated_air_gs, backpressure_kpa):
    # Hot exhaust (~900 K) volume flow; k_carb rating for the target back-pressure
    # dp = 5.08 kPa (Q/k)^2. GX390: Honda allowable 6.0-12.5 kPa at rated (F).
    rho_hot = 101325 / (287.0 * 900.0) * 1000.0               # g/m3 -> g/L below
    q_cfm = rated_air_gs * 1.07 / (rho_hot / 1000.0) / 0.471947
    k = q_cfm * math.sqrt(5.08 / backpressure_kpa)
    return d(k, q_cfm * math.sqrt(5.08 / (backpressure_kpa * 1.6)), q_cfm * math.sqrt(5.08 / (backpressure_kpa * 0.6)),
             'PROJECT (Honda back-pressure window)')

def rated_backpressure_kpa(cylinders):
    return d(9.0 if cylinders <= 2 else 12.0, 5.0, 18.0, 'C / Honda GX tech manual window 6-12.5')

def plenum_volume_l(displacement_l, cylinders):
    k = 0.75 if cylinders == 1 else 1.25
    return d(k * displacement_l, 0.5 * displacement_l, 2.0 * displacement_l, 'C (GX390 0.77 x, Mazda 1.25 x)')

def intake_runner_length_mm(tuning_rpm):
    # Tuned intake length scales ~1/rpm (wave/Helmholtz tuning, Heywood ch. 7):
    # ~400-450 mm for a ~4000 rpm peak-torque 2 L (Mazda class), longer for
    # slower engines; small industrial engines carry short ports + carb (the
    # inertial column is then mostly the port). Placeholder C value anchored
    # at 425 mm @ 4000 rpm; range 0.5-1.5 x. To be refined from sources.
    L = 425.0 * 4000.0 / max(tuning_rpm, 1000.0)
    return d(L, 0.5 * L, 1.5 * L, 'C (1/rpm tuning scaling, anchored Mazda-class 425 mm @ 4000)')

# --- mixture and fuel ------------------------------------------------------
def full_load_lambda(fuel_system):
    if fuel_system == 'carburettor':
        return d(0.90, 0.85, 0.96, 'PROJECT GX390 UA data 0.88-0.96 (F); Kohler cycle 0.83-0.87')
    return d(0.95, 0.85, 1.00, 'C (EFI WOT enrichment; Mazda EPA 0.84-1.0 D)')

# --- calibration knobs -------------------------------------------------------
PORT_CD = d(0.60, 0.45, 0.75, 'C shared head discharge coefficient (Deere/GX390/Mazda 0.6)')
