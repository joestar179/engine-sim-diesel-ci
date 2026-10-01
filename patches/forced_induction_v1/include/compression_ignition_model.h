#ifndef ATG_ENGINE_SIM_COMPRESSION_IGNITION_MODEL_H
#define ATG_ENGINE_SIM_COMPRESSION_IGNITION_MODEL_H

class CompressionIgnitionModel {
public:
    struct Parameters {
        bool enabled = false;
        double maxFuelMassPerCycle = 0.0;      // kg per cylinder event
        double injectionDuration = 0.0;        // crank radians
        double ignitionDelay = 0.0;            // crank radians
        double combustionDuration = 0.0;       // crank radians
        double premixedBurnFraction = 0.20;    // 0..1
        double autoignitionTemperature = 550.0;  // K
        double autoignitionPressure = 1.0e6;   // Pa
        // true: ignition delay from the cylinder state (Livengood-Wu integral
        // of the Assanis et al. 2003 DI-diesel correlation); ignitionDelay
        // is then unused. false: fixed crank-angle ignitionDelay.
        bool ignitionDelayCorrelation = false;
    };

    struct Event {
        bool active = false;
        bool combustionStarted = false;
        double elapsed = 0.0;
        double injectionDurationSeconds = 0.0;
        double ignitionDelaySeconds = 0.0;
        double combustionDurationSeconds = 0.0;
        double targetFuelMoles = 0.0;
        double injectedFuelMoles = 0.0;
        double demandedBurnFuelMoles = 0.0;
        double equivalenceRatio = 0.5;      // overall, set by the chamber
        double ignitionIntegral = 0.0;      // Livengood-Wu: ignition at 1
        double ignitionTime = -1.0;         // s after start of injection
    };

    struct StepResult {
        double fuelMolesToInject = 0.0;
        double fuelMolesToBurn = 0.0;
        bool combustionStarted = false;
        bool active = false;
    };

    CompressionIgnitionModel();

    void initialize(const Parameters &parameters);
    const Parameters &parameters() const { return m_parameters; }
    bool enabled() const { return m_parameters.enabled; }

    void beginEvent(Event &event, double fuelMass, double fuelMolecularMass, double engineSpeedRadPerSec) const;
    StepResult step(Event &event, double dt, double cylinderTemperature, double cylinderPressure) const;

    static double wiebe(double normalizedProgress, double a, double m);
    // Assanis et al. (2003): tau = 2.4 phi^-0.2 p^-1.02 exp(2100/T) ms,
    // p in bar, T in K. Returns seconds.
    static double ignitionDelayTime(double temperature, double pressure, double equivalenceRatio);
    double cumulativeBurnFraction(double normalizedProgress) const;

private:
    Parameters m_parameters;
};

#endif
