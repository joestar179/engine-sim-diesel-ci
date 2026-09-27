#ifndef ATG_ENGINE_SIM_TURBO_GROUP_H
#define ATG_ENGINE_SIM_TURBO_GROUP_H

#include "gas_system.h"
#include "turbocharger_model.h"
#include "units.h"

#include <vector>

class ExhaustSystem;

// Generic forced-induction gas/shaft group. The object is deliberately
// combustion-agnostic: it only owns air/exhaust gas state, flow devices and
// one turbo shaft. Engine control/fueling remains outside this class.
class TurboGroup {
public:
    struct Parameters {
        bool enabled = false;
        int inletChannelCount = 1;

        double preTurbineVolume = units::volume(0.001, units::m3);
        double preTurbineArea = units::area(0.001, units::m2);
        double compressorInletVolume = units::volume(0.001, units::m3);
        double compressorDischargeVolume = units::volume(0.001, units::m3);
        double chargePlenumVolume = units::volume(0.001, units::m3);
        double chargeArea = units::area(0.001, units::m2);

        double inletFlowRate = 0.0;
        double passiveCompressorFlowRate = 0.0;
        double coolerFlowRate = 0.0;

        bool wastegateEnabled = false;
        double wastegateFlowRate = 0.0;
        double wastegatePosition = 0.0;
        double wastegateResponseRate = 0.0;

        bool compressorBypassEnabled = false;
        double compressorBypassFlowRate = 0.0;
        double compressorBypassPosition = 0.0;
        double compressorBypassResponseRate = 0.0;
        bool compressorBypassRecirculates = true;

        bool vgtEnabled = false;
        double vgtPosition = 1.0;
        double vgtMinFlowFactor = 0.25;
        double vgtResponseRate = 0.0;
    };

    struct Telemetry {
        double compressorMassFlow = 0.0;
        double compressorPressureRatio = 1.0;
        double compressorPower = 0.0;
        double compressorDischargeTemperature = units::celcius(25.0);
        double chargePressure = units::pressure(1.0, units::atm);
        double chargeTemperature = units::celcius(25.0);

        double turbineMassFlow = 0.0;
        double wastegateMassFlow = 0.0;
        double turbinePressureRatio = 1.0;
        double turbinePower = 0.0;
        double turbineInletPressure = units::pressure(1.0, units::atm);
        double turbineInletTemperature = units::celcius(25.0);
        double turbineOutletPressure = units::pressure(1.0, units::atm);
        double extractedTurbineEnergy = 0.0;
        double shaftSpeed = 0.0;
        double bypassMassFlow = 0.0;
    };

public:
    TurboGroup();

    void initialize(
        const TurbochargerModel::Parameters &turboParameters,
        const Parameters &parameters);
    void reset();

    bool enabled() const { return m_parameters.enabled && m_model.enabled(); }
    int inletChannelCount() const { return static_cast<int>(m_preTurbine.size()); }

    TurbochargerModel *model() { return &m_model; }
    const TurbochargerModel *model() const { return &m_model; }

    GasSystem *preTurbineSystem(int channel);
    const GasSystem *preTurbineSystem(int channel) const;
    GasSystem *chargePlenum() { return &m_chargePlenum; }
    const GasSystem *chargePlenum() const { return &m_chargePlenum; }

    double preTurbineAreaPerChannel() const;

    void setPostTurbineExhaust(ExhaustSystem *system) { m_postTurbineExhaust = system; }
    ExhaustSystem *postTurbineExhaust() const { return m_postTurbineExhaust; }

    // Compressor and turbine are called once per fluid substep. Compressor
    // work is accumulated and debited from the same shaft when processTurbine
    // advances the shaft state later in that substep.
    void processCompressor(double dt);
    void processTurbine(double dt);

    const Telemetry &telemetry() const { return m_telemetry; }
    const Parameters &parameters() const { return m_parameters; }

private:
    static double advanceActuator(double current, double target, double rate, double dt);
    double effectiveTurbineFlowK() const;
    double totalPreTurbinePressure() const;
    double totalPreTurbineTemperature() const;

private:
    Parameters m_parameters;
    TurbochargerModel m_model;

    GasSystem m_ambient;
    GasSystem m_compressorInlet;
    GasSystem m_compressorDischarge;
    GasSystem m_chargePlenum;
    std::vector<GasSystem> m_preTurbine;

    ExhaustSystem *m_postTurbineExhaust = nullptr;

    double m_turbineFlowK = 0.0;
    double m_wastegatePosition = 0.0;
    double m_bypassPosition = 0.0;
    double m_vgtPosition = 1.0;

    double m_pendingCompressorMassFlow = 0.0;
    double m_pendingCompressorPower = 0.0;
    double m_pendingCompressorEnergy = 0.0;
    double m_pendingCompressorPressureRatio = 1.0;
    double m_pendingCompressorDischargeTemperature = units::celcius(25.0);

    Telemetry m_telemetry;
};

#endif /* ATG_ENGINE_SIM_TURBO_GROUP_H */
