"""Spec sheet -> Engine Simulator .mr (naturally aspirated spark ignition, v0).

Layer-2 defaults (defaults.py) fill everything the spec sheet does not give;
documented values in the spec override defaults. Knobs (port_cd, spark curve,
lambda) are script inputs so calibrate.py can set them without regenerating.

Spec keys (documented values; anything else is defaulted):
  name, layout ('single' | 'v2_90' | 'i4'), bore_mm, stroke_mm, cr,
  valves_per_cyl, valvetrain (defaults.valvetrain_friction keys),
  fuel_system ('carburettor' | 'efi'), rated_kw, rated_rpm, peak_nm, peak_rpm,
  idle_rpm, redline_rpm, rating_cf (power-correction factor applied to sim
  output, e.g. SAE J1349 0.973 at 101.325 kPa), fuel ('gasoline' | 'tier2')
  optional documented overrides: intake_valve_mm, exhaust_valve_mm,
  intake_lift_mm, exhaust_lift_mm, intake_duration, exhaust_duration,
  intake_center, exhaust_center, rod_mm, lambda, backpressure_kpa
"""
import json, math, sys
import defaults as D

LAYOUTS = {
    # bank angles (deg), cylinder -> (bank, journal angle deg, firing phase deg), crank tdc (deg)
    'single': dict(banks=[0.0], cyl=[(0, 0.0, 0.0)], tdc=90.0),
    'v2_90':  dict(banks=[-45.0, 45.0], cyl=[(0, 0.0, 0.0), (1, 0.0, 270.0)], tdc=45.0),
    'i4':     dict(banks=[0.0], cyl=[(0, 0.0, 0.0), (0, 540.0, 540.0), (0, 180.0, 180.0), (0, 360.0, 360.0)], tdc=90.0),
}

def resolve(spec):
    """Return (inputs, provenance): every value with its grade/source."""
    v, prov = {}, {}
    def put(key, documented, default):
        if documented is not None:
            v[key] = documented; prov[key] = 'documented'
        else:
            v[key] = default['value']; prov[key] = '%s [%.4g..%.4g]' % (default['source'], default['low'], default['high'])
    nv = spec.get('valves_per_cyl', 2)
    put('intake_valve_mm', spec.get('intake_valve_mm'), D.intake_valve_diameter(spec['bore_mm'], nv))
    put('exhaust_valve_mm', spec.get('exhaust_valve_mm'), D.exhaust_valve_diameter(v['intake_valve_mm'], nv))
    put('intake_lift_mm', spec.get('intake_lift_mm'), D.valve_lift(v['intake_valve_mm']))
    put('exhaust_lift_mm', spec.get('exhaust_lift_mm'), D.valve_lift(v['exhaust_valve_mm']))
    cam = D.cam_class(spec['rated_rpm'])
    for k in ('intake_duration', 'exhaust_duration', 'intake_center', 'exhaust_center'):
        put(k, spec.get(k), cam[k])
    put('rod_mm', spec.get('rod_mm'), D.rod_length(spec['stroke_mm']))
    b = D.bearings(spec['bore_mm'])
    for k in b: put(k + '_mm', spec.get(k + '_mm'), b[k])
    ncyl = len(LAYOUTS[spec['layout']]['cyl'])
    disp_l = math.pi / 4 * (spec['bore_mm'] / 1000) ** 2 * (spec['stroke_mm'] / 1000) * ncyl * 1000
    v['displacement_l'] = disp_l
    air_gs = D.rated_airflow_gs(disp_l, spec['rated_rpm'])
    put('intake_cfm', spec.get('intake_cfm'), D.intake_restriction_cfm(D.gs_to_cfm(air_gs)))
    bp = D.rated_backpressure_kpa(ncyl)
    put('backpressure_kpa', spec.get('backpressure_kpa'), bp)
    put('exhaust_cfm', spec.get('exhaust_cfm'), D.exhaust_outlet_cfm(air_gs, v['backpressure_kpa']))
    put('plenum_l', spec.get('plenum_l'), D.plenum_volume_l(disp_l, ncyl))
    put('lambda', spec.get('lambda'), D.full_load_lambda(spec['fuel_system']))
    put('port_cd', spec.get('port_cd'), D.PORT_CD)
    vt, src = D.valvetrain_friction(spec['valvetrain'])
    v['vt_friction'] = vt; prov['vt_friction'] = src
    return v, prov

def flow_table(valve_mm, n_valves, max_lift_mm):
    # Geometry rule (Deere/GX390/Mazda): CFM @28 inH2O per mm2 = 0.2272,
    # min(curtain, throat 0.88 d) per valve, x valves; cd applied in the script.
    thr = n_valves * math.pi / 4 * (0.88 * valve_mm) ** 2
    rows = []
    top = int(max_lift_mm / 0.0254 / 50 + 2) * 50
    for L in range(0, top + 1, 50):
        cur = n_valves * math.pi * valve_mm * L * 0.0254
        rows.append('        .add_flow_sample(%d, %.1f * cd)' % (L, 0.2272 * min(cur, thr)))
    return '\n'.join(rows)

def generate(spec, out_path):
    v, prov = resolve(spec)
    L = LAYOUTS[spec['layout']]
    ncyl = len(L['cyl'])
    nv = spec.get('valves_per_cyl', 2)
    n_in = 2 if nv >= 4 else 1
    node = spec['node']
    fuel = {'gasoline': ('Gasoline [LHV 43.4 MJ/kg]', 43.4, 0.745),
            'tier2': ('EPA Tier 2 gasoline [LHV 42.887 MJ/kg]', 42.887, 0.743)}[spec.get('fuel', 'gasoline')]
    vt = v['vt_friction']
    disp_cc = v['displacement_l'] * 1000 / ncyl
    chamber = disp_cc / (spec['cr'] - 1)
    wires = '\n'.join('    output wire%d: ignition_wire();' % (i + 1) for i in range(ncyl))
    pins = []
    for c in L['cyl']:
        if c[1] not in pins: pins.append(c[1])
    pin_of = [pins.index(c[1]) + 1 for c in L['cyl']]
    journals = '\n'.join('    rod_journal rj%d(angle: %.1f * units.deg)' % (j + 1, a) for j, a in enumerate(pins))
    add_j = '\n'.join('        .add_rod_journal(rj%d)' % (j + 1) for j in range(len(pins)))
    lobes_ex = '\n'.join('        .add_lobe((rot360 - exhaust_lobe_center) + %.1f * units.deg)' % c[2] for c in L['cyl'])
    lobes_in = '\n'.join('        .add_lobe(rot360 + intake_lobe_center + %.1f * units.deg)' % c[2] for c in L['cyl'])
    banks, heads = [], []
    for bi, ang in enumerate(L['banks']):
        cyls = [i for i, c in enumerate(L['cyl']) if c[0] == bi]
        adds = '\n'.join(('        .add_cylinder(\n            piston: piston(piston_params, blowby: k_28inH2O(0.05)),\n'
                          '            connecting_rod: connecting_rod(rod_params),\n'
                          '            rod_journal: rj%d, intake: intake, exhaust_system: exhaust0,\n'
                          '            ignition_wire: wires.wire%d, primary_length: 300 * units.mm)') % (pin_of[i], i + 1) for i in cyls)
        banks.append('    cylinder_bank b%d(bank_params, angle: %.1f * units.deg)\n    b%d\n%s\n    engine.add_cylinder_bank(b%d)' % (bi, ang, bi, adds, bi))
        # Each bank's head sees the shared camshaft; lobe index = cylinder index within the bank order.
        heads.append(('    b%d.set_cylinder_head(generic_cylinder_head(\n'
                      '        chamber_volume: %.3f * units.cc,\n'
                      '        intake_runner_volume: %.1f * units.cc,\n'
                      '        intake_runner_cross_section_area: %.2f * units.cm2,\n'
                      '        exhaust_runner_volume: %.1f * units.cc,\n'
                      '        exhaust_runner_cross_section_area: %.2f * units.cm2,\n'
                      '        intake_port_flow: %s_intake_flow(cd: port_cd),\n'
                      '        exhaust_port_flow: %s_exhaust_flow(cd: port_cd),\n'
                      '        valvetrain: standard_valvetrain(intake_camshaft: cams%d.intake_cam, exhaust_camshaft: cams%d.exhaust_cam)%s))')
                     % (bi, chamber, 0.3 * disp_cc, n_in * math.pi / 4 * (v['intake_valve_mm'] / 10) ** 2,
                        0.2 * disp_cc, n_in * math.pi / 4 * (v['exhaust_valve_mm'] / 10) ** 2,
                        node, node, bi, bi, ',\n        flip_display: true' if bi == 1 else ''))
    cams = []
    for bi in range(len(L['banks'])):
        cyls = [c for c in L['cyl'] if c[0] == bi]
        li = '\n'.join('        .add_lobe(rot360 + intake_lobe_center + %.1f * units.deg)' % c[2] for c in cyls)
        le = '\n'.join('        .add_lobe((rot360 - exhaust_lobe_center) + %.1f * units.deg)' % c[2] for c in cyls)
        cams.append(('    %s_camshafts cams%d(intake_lobe_center: intake_center, exhaust_lobe_center: exhaust_center,\n'
                     '        intake_lobe_profile: intake_lobe, exhaust_lobe_profile: exhaust_lobe)') % (node + '_b%d' % bi, bi))
    cam_nodes = []
    for bi in range(len(L['banks'])):
        cyls = [c for c in L['cyl'] if c[0] == bi]
        cam_nodes.append('''private node %s_b%d_camshafts {
    input intake_lobe_profile;
    input exhaust_lobe_profile;
    input intake_lobe_center;
    input exhaust_lobe_center;
    output intake_cam: _intake_cam;
    output exhaust_cam: _exhaust_cam;
    camshaft_parameters params(advance: 0 * units.deg, base_radius: 15 * units.mm)
    camshaft _intake_cam(params, lobe_profile: intake_lobe_profile)
    camshaft _exhaust_cam(params, lobe_profile: exhaust_lobe_profile)
    label rot360(360 * units.deg)
    _exhaust_cam
%s
    _intake_cam
%s
}
''' % (node, bi, '\n'.join('        .add_lobe((rot360 - exhaust_lobe_center) + %.1f * units.deg)' % c[2] for c in cyls),
       '\n'.join('        .add_lobe(rot360 + intake_lobe_center + %.1f * units.deg)' % c[2] for c in cyls)))
    wires_ign = '\n'.join('            .connect_wire(wires.wire%d, %.1f * units.deg)' % (i + 1, c[2]) for i, c in enumerate(L['cyl']))
    prov_txt = '\n'.join('      %-18s %s' % (k, prov[k]) for k in sorted(prov))
    mr = f'''import "engine_sim.mr"

units units()
constants constants()
impulse_response_library ir_lib()

/*
    {spec['name']} — generated by tools/engine_setup/generate.py (Layer-2
    defaults; CLAUDE.md 0e). Do not hand-edit: regenerate from the spec.
    Spec: {json.dumps({k: spec[k] for k in spec if k != 'name'})}
    Provenance of every non-spec value:
{prov_txt}
    Knobs (script inputs): port_cd (breathing), spark_* (MBT curve), lambda.
*/

private node {node}_wires {{
{wires}
}}

private node {node}_intake_flow {{
    input cd;
    alias output __out: f;
    function f(50 * units.thou)
    f
{flow_table(v['intake_valve_mm'], n_in, v['intake_lift_mm'])}
}}

private node {node}_exhaust_flow {{
    input cd;
    alias output __out: f;
    function f(50 * units.thou)
    f
{flow_table(v['exhaust_valve_mm'], n_in, v['exhaust_lift_mm'])}
}}

{''.join(cam_nodes)}
public node {node} {{
    input port_cd: {v['port_cd']};
    input lambda: {v['lambda']};
    input spark_1000: 15 * units.deg;
    input spark_2000: 20 * units.deg;
    input spark_3000: 25 * units.deg;
    input spark_4000: 28 * units.deg;
    input spark_5000: 30 * units.deg;
    input spark_6000: 32 * units.deg;
    input spark_7000: 34 * units.deg;
    input fuel_energy: {fuel[1]} * units.kJ / units.g;
    input engine_name: "{spec['name']}";
    alias output __out: engine;

    input intake_center: {v['intake_center']} * units.deg;   // tuning knob (range in provenance)
    label exhaust_center({v['exhaust_center']} * units.deg)

    engine engine(
        name: engine_name,
        starter_torque: {max(20, 25 * v['displacement_l']):.0f} * units.lb_ft,
        starter_speed: 400 * units.rpm,
        redline: {spec.get('redline_rpm', spec['rated_rpm'] * 1.1):.0f} * units.rpm,
        fuel: fuel(name: "{fuel[0]}", energy_density: fuel_energy, density: {fuel[2]} * units.kg / units.L,
                   max_burning_efficiency: 0.97, burning_efficiency_randomness: 0.0),
        component_friction: true,
        main_bearing_count: {spec.get('main_bearings', ncyl + 1)},
        main_bearing_diameter: {v['main_d_mm']:.2f} * units.mm,
        main_bearing_length: {v['main_l_mm']:.2f} * units.mm,
        rod_bearing_diameter: {v['rod_d_mm']:.2f} * units.mm,
        rod_bearing_length: {v['rod_l_mm']:.2f} * units.mm,
        cam_bearing_count: {2 if nv < 4 else 2 * (ncyl + 1)},
        valve_count: {ncyl * nv},
        max_valve_lift: {v['intake_lift_mm']:.2f} * units.mm,
        valvetrain_flat_follower: {vt[0]},
        valvetrain_roller_follower: {vt[1]},
        valvetrain_oscillating_hydrodynamic: {vt[2]},
        valvetrain_oscillating_mixed: {vt[3]},
        oil_viscosity_ratio: 1.0,
        simulation_frequency: 10000,
        hf_gain: 0.01,
        noise: 1.0,
        jitter: 0.1
    )

    {node}_wires wires()

    crankshaft c0(
        throw: {spec['stroke_mm'] / 2:.3f} * units.mm,
        flywheel_mass: {3 + 3 * v['displacement_l']:.1f} * units.kg,
        mass: {2 + 5 * v['displacement_l']:.1f} * units.kg,
        friction_torque: 0.0 * units.lb_ft,
        moment_of_inertia: {0.03 + 0.06 * v['displacement_l']:.3f},
        position_x: 0.0,
        position_y: 0.0,
        tdc: {L['tdc']:.1f} * units.deg
    )
{journals}
    c0
{add_j}

    piston_parameters piston_params(
        mass: {0.00045 * spec['bore_mm'] ** 3:.1f} * units.g,     // C (GX390 250 g, Mazda 300 g class)
        compression_height: {0.3 * spec['bore_mm']:.2f} * units.mm,
        wrist_pin_position: 0.0,
        displacement: 0.0
    )
    connecting_rod_parameters rod_params(
        mass: {0.0004 * spec['bore_mm'] ** 3:.1f} * units.g,      // C
        moment_of_inertia: 0.0015,
        center_of_mass: 0.0,
        length: {v['rod_mm']:.2f} * units.mm
    )
    cylinder_bank_parameters bank_params(
        bore: {spec['bore_mm']} * units.mm,
        deck_height: {v['rod_mm'] + spec['stroke_mm'] / 2 + 0.3 * spec['bore_mm']:.2f} * units.mm
    )

    intake intake(
        plenum_volume: {v['plenum_l']:.3f} * units.L,
        plenum_cross_section_area: {math.pi * (math.sqrt(v['intake_cfm'] / 373.0) * 26) ** 2 / 100:.2f} * units.cm2,
        intake_flow_rate: k_carb({v['intake_cfm']:.1f}),
        idle_flow_rate: k_carb(0.0),
        idle_throttle_plate_position: 0.99,
        throttle_gamma: 1.0,
        molecular_afr: lambda / 0.064,
        velocity_decay: 1.0
    )

    exhaust_system_parameters es_params(
        outlet_flow_rate: k_carb({v['exhaust_cfm']:.1f}),
        primary_tube_length: 300 * units.mm,
        primary_flow_rate: k_carb(300.0),
        velocity_decay: 1.0,
        volume: {max(1.5, 2.0 * v['displacement_l']):.2f} * units.L
    )
    exhaust_system exhaust0(es_params, audio_volume: 1.0, impulse_response: ir_lib.mild_exhaust_0)

{chr(10).join(banks)}
    engine.add_crankshaft(c0)

    harmonic_cam_lobe intake_lobe(duration_at_50_thou: {v['intake_duration']:.1f} * units.deg, gamma: 1.1,
        lift: {v['intake_lift_mm']:.2f} * units.mm, steps: 100)
    harmonic_cam_lobe exhaust_lobe(duration_at_50_thou: {v['exhaust_duration']:.1f} * units.deg, gamma: 1.1,
        lift: {v['exhaust_lift_mm']:.2f} * units.mm, steps: 100)
{chr(10).join(cams)}
{chr(10).join(heads)}

    function timing_curve(1000 * units.rpm)
    timing_curve
        .add_sample(0 * units.rpm, spark_1000)
        .add_sample(1000 * units.rpm, spark_1000)
        .add_sample(2000 * units.rpm, spark_2000)
        .add_sample(3000 * units.rpm, spark_3000)
        .add_sample(4000 * units.rpm, spark_4000)
        .add_sample(5000 * units.rpm, spark_5000)
        .add_sample(6000 * units.rpm, spark_6000)
        .add_sample(7000 * units.rpm, spark_7000)
        .add_sample(8000 * units.rpm, spark_7000)

    engine.add_ignition_module(
        ignition_module(timing_curve: timing_curve, rev_limit: {spec.get('redline_rpm', spec['rated_rpm'] * 1.1) + 200:.0f} * units.rpm)
{wires_ign})
}}
'''
    open(out_path, 'w', encoding='utf-8').write(mr)
    return v, prov

if __name__ == '__main__':
    spec = json.load(open(sys.argv[1]))
    v, prov = generate(spec, sys.argv[2])
    for k in sorted(prov): print('%-18s %-10s %s' % (k, ('%.4g' % v[k]) if isinstance(v[k], (int, float)) else v[k], prov[k]))
