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

        // Injection and combustion from the injection hardware (used when
        // nozzleHoles > 0; otherwise the prescribed injection/combustion
        // durations above are used). One combustion framework, two
        // injection systems:
        //   common rail     rail pressure sets the nozzle flow:
        //                   m' = Cd n (pi/4 d^2) sqrt(2 rho_f (p_rail - p_cyl))
        //   mechanical pump the plunger sets the delivery, proportional to
        //                   engine speed: Q = n_p (pi/4 d_p^2) (dh/dtheta) w;
        //                   hole velocity by continuity v = Q / (Cd A_holes)
        //                   (the opening pressure only lifts the needle)
        //   burn rate       (injected - burned) x u / L, L = bore / 2,
        //                   u = 0.5 x mean piston speed (the turbulence of
        //                   the spark-ignition flame model) + C_s x spray
        //                   velocity (decaying after end of injection)
        int nozzleHoles = 0;
        double nozzleHoleDiameter = 0.0;        // m
        double injectionPressure = 0.0;         // Pa: rail (common rail) or nozzle opening (pump)
        double nozzleDischargeCoefficient = 0.7;
        double sprayTurbulenceCoefficient = 0.0;
        int pumpPlungers = 0;                   // > 0 selects the mechanical pump
        double pumpPlungerDiameter = 0.0;       // m
        double pumpCamLiftRate = 0.0;           // m of plunger lift per crank radian
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

        // Hardware-driven path (set by the chamber at the start of the event).
        double fuelDensity = 0.0;           // kg/m^3
        double fuelMolecularMass = 0.0;     // kg/mol
        double lengthScale = 0.0;           // m, bore / 2
        double pistonTurbulence = 0.0;      // m/s, 0.5 x mean piston speed
        double sprayVelocity = 0.0;         // m/s
        double maxDuration = 0.0;           // s, abandon an unlit event
        double omega = 0.0;                 // rad/s, crank speed at start
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
    bool usesMechanicalPump() const {
        return m_parameters.pumpPlungers > 0 && m_parameters.pumpPlungerDiameter > 0.0 && m_parameters.pumpCamLiftRate > 0.0;
    }
    bool usesInjectionHardware() const {
        return m_parameters.nozzleHoles > 0 && m_parameters.nozzleHoleDiameter > 0.0
            && (usesMechanicalPump() || m_parameters.injectionPressure > 0.0);
    }

    void beginEvent(Event &event, double fuelMass, double fuelMolecularMass, double engineSpeedRadPerSec) const;
    StepResult step(Event &event, double dt, double cylinderTemperature, double cylinderPressure) const;

    static double wiebe(double normalizedProgress, double a, double m);
    StepResult stepHardware(Event &event, double dt, double cylinderTemperature, double cylinderPressure) const;
    // Assanis et al. (2003): tau = 2.4 phi^-0.2 p^-1.02 exp(2100/T) ms,
    // p in bar, T in K. Returns seconds.
    static double ignitionDelayTime(double temperature, double pressure, double equivalenceRatio);
    double cumulativeBurnFraction(double normalizedProgress) const;

private:
    Parameters m_parameters;
};

#endif
