from pathlib import Path
p=Path('assets/engines/alco/alco_251d_diesel_turbo.mr')
s=p.read_text(encoding='utf-8')
anchor='        turbo_design_mass_flow: 1.45 * units.kg / units.sec,\n'
insert=anchor+'''\n        // GENERIC FORCED-INDUCTION INSTALLATION GEOMETRY. These are
        // CALIBRATION/estimated values until 251-D/350B dimensions are sourced.
        // The existing 110-inch ExhaustSystem remains downstream of the turbine.
        turbo_pre_turbine_volume: 20 * units.L,
        turbo_pre_turbine_area: circle_area(8 * units.inch),
        turbo_inlet_channel_count: 1,
        turbo_compressor_inlet_volume: 20 * units.L,
        turbo_compressor_discharge_volume: 15 * units.L,
        turbo_charge_plenum_volume: 50 * units.L,
        turbo_charge_area: 400 * units.cm2,
        aftercooler_enabled: true,
        // Zero asks the generic model to derive stable flow capacities from
        // turbo_design_mass_flow rather than imposing arbitrary airflow.
        turbo_inlet_flow_rate: 0.0,
        turbo_passive_compressor_flow_rate: 0.0,
        turbo_cooler_flow_rate: 0.0,
        wastegate_enabled: false,
        wastegate_flow_rate: 0.0,
        wastegate_position: 0.0,
        compressor_bypass_enabled: false,
        compressor_bypass_flow_rate: 0.0,
        compressor_bypass_position: 0.0,
        compressor_bypass_recirculates: true,
        vgt_enabled: false,
        vgt_position: 1.0,
        vgt_min_flow_factor: 0.25,
'''
if 'turbo_pre_turbine_volume:' not in s:
    if anchor not in s: raise SystemExit('ALCO generic turbo configuration anchor missing')
    s=s.replace(anchor,insert,1)
p.write_text(s,encoding='utf-8',newline='\n')
print('ALCO validation engine configured for generic forced-induction V1')
