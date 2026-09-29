#ifndef ATG_ENGINE_SIM_FUEL_RACK_GOVERNOR_MODEL_H
#define ATG_ENGINE_SIM_FUEL_RACK_GOVERNOR_MODEL_H

class FuelRackGovernorModel {
public:
    struct Parameters {
        double minSpeed = 1.0;       // rad/s
        double maxSpeed = 1.0;       // rad/s
        double minRackRate = -2.0;   // rack units/s
        double maxRackRate = 2.0;
        double k_s = 1.0;
        double k_d = 300.0;
        double gamma = 1.0;
        double startingRack = 0.20;
        // Proportional compensation (rack per unit of normalized speed error,
        // (target - speed) / target). 0 keeps the original integral-only law.
        double k_p = 0.0;
        // Output-rack ceiling from standstill until the engine first reaches
        // minimum (idle) speed: the start-fuel limit. It re-arms when the
        // engine falls back below half minimum speed (a stall or restart).
        // 1 keeps the original unlimited behaviour.
        double crankRackLimit = 1.0;
    };

    FuelRackGovernorModel();

    void initialize(const Parameters &parameters);
    void reset();
    void setSpeedControl(double normalizedControl);
    double update(double dt, double engineSpeedRadPerSec);

    double targetSpeed() const { return m_targetSpeed; }
    double rack() const { return m_rack; }
    const Parameters &parameters() const { return m_parameters; }

private:
    Parameters m_parameters;
    double m_speedControl = 0.0;
    double m_targetSpeed = 0.0;
    double m_rack = 0.0;
    double m_rackRate = 0.0;
    double m_output = 0.0;
    bool m_starting = true;
};

#endif
