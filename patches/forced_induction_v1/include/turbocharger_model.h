#ifndef ATG_ENGINE_SIM_TURBOCHARGER_MODEL_H
#define ATG_ENGINE_SIM_TURBOCHARGER_MODEL_H

class TurbochargerModel {
public:
    struct Parameters {
        bool enabled = false;
        double shaftInertia = 1.0;              // kg m^2
        double frictionTorque = 0.0;            // N m
        double maxSpeed = 1.0;                  // rad/s
        double maxPressureRatio = 1.0;
        double compressorEfficiency = 0.70;
        double turbineEfficiency = 0.70;
        double aftercoolerEffectiveness = 0.0;
        double designMassFlow = 1.0;             // kg/s, whole turbo
        double turbineDesignPressureRatio = 1.8;
        double turbineDesignTemperature = 800.0; // K
    };

    struct CompressorPoint {
        double massFlow = 0.0;                  // kg/s for this intake share
        double pressureRatio = 1.0;
        double outletPressure = 101325.0;
        double compressorOutletTemperature = 298.15;
        double chargeTemperature = 298.15;
        double power = 0.0;                     // shaft power consumed, W
    };

    struct Inputs {
        double dt = 0.0;
        double intakeMassFlow = 0.0;
        double compressorPowerDemand = 0.0;
        double exhaustMassFlow = 0.0;
        double exhaustPressure = 101325.0;
        double exhaustTemperature = 298.15;
        double turbineOutletPressure = 101325.0;
        double ambientPressure = 101325.0;
        double ambientTemperature = 298.15;
        double maxExtractableExhaustEnergy = 0.0;
    };

    struct Output {
        double shaftSpeed = 0.0;
        double pressureRatio = 1.0;
        double chargePressure = 101325.0;
        double chargeTemperature = 298.15;
        double compressorPower = 0.0;
        double turbinePower = 0.0;
        double turbinePressureRatio = 1.0;
        double extractedTurbineEnergy = 0.0;
        double rejectedTurbineEnergy = 0.0;
    };

    TurbochargerModel();

    void initialize(const Parameters &parameters);
    void reset();
    const Parameters &parameters() const { return m_parameters; }
    bool enabled() const { return m_parameters.enabled; }
    double shaftSpeed() const { return m_speed; }
    double shaftEnergy() const;

    Output step(const Inputs &inputs);
    void advanceShaft(double dt, double turbinePower, double compressorPower);

    double pressureRatioFor(double speed, double massFlow) const;
    double compressorOutletTemperature(double pressureRatio, double ambientTemperature) const;
    CompressorPoint compressorOperatingPoint(
        double downstreamPressure,
        double ambientPressure,
        double ambientTemperature,
        double flowShare = 1.0) const;

private:
    Parameters m_parameters;
    double m_speed = 0.0;
};

#endif

