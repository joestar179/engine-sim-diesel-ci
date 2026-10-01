#ifndef ATG_ENGINE_SIM_ENGINE_FRICTION_MODEL_H
#define ATG_ENGINE_SIM_ENGINE_FRICTION_MODEL_H

// Component mechanical friction model of Patton, Nitschke and Heywood
// (SAE 890836), as given in D. Sandoval, "An Improved Friction Model for
// Spark Ignition Engines", MIT 2003, pp. 13-16 and Appendix A.2 (original
// PNH coefficients; Sandoval's oil-viscosity scaling applied to the
// hydrodynamic terms). Terms:
//   crankshaft     main-bearing seals, main bearings, turbulent dissipation
//   reciprocating  piston skirt, rings (mixed), rod bearings
//   gas loading    ring friction from cylinder gas pressure
//   valvetrain     cam bearings (+ boundary constant), follower, oscillating
//   auxiliary      oil pump, water pump, non-charging alternator (fitted on
//                  small high-speed diesel engines)
// Pumping (gas exchange) is not included: the simulator computes it.
// All lengths in mm, speed in rpm, mean piston speed in m/s, fmep in kPa.
class EngineFrictionModel {
public:
    struct Parameters {
        bool enabled = false;

        // From the engine geometry.
        int cylinders = 0;
        double bore = 0.0;               // mm
        double stroke = 0.0;             // mm
        double compressionRatio = 0.0;

        // From the script (bearing and valvetrain geometry).
        int mainBearings = 0;
        double mainBearingDiameter = 0.0;   // mm
        double mainBearingLength = 0.0;     // mm
        double rodBearingDiameter = 0.0;    // mm
        double rodBearingLength = 0.0;      // mm
        int camBearings = 0;
        int valves = 0;                     // total
        double maxValveLift = 0.0;          // mm

        // Valvetrain mechanism constants (Sandoval table 4.2):
        // flat follower, roller follower, oscillating hydrodynamic and
        // oscillating mixed. A flat-follower engine uses rollerFollower = 0.
        double flatFollower = 0.0;
        double rollerFollower = 0.0;
        double oscillatingHydrodynamic = 0.0;
        double oscillatingMixed = 0.0;

        // Oil kinematic viscosity / PNH reference (10W-30, 10.6 cSt at 90 C).
        double viscosityRatio = 1.0;
    };

    struct Breakdown {
        double crankshaft = 0.0;
        double reciprocating = 0.0;
        double gasLoading = 0.0;
        double valvetrain = 0.0;
        double auxiliary = 0.0;
        double total = 0.0;              // kPa
    };

    void initialize(const Parameters &parameters) { m_parameters = parameters; }
    bool enabled() const { return m_parameters.enabled && m_parameters.cylinders > 0; }
    const Parameters &parameters() const { return m_parameters; }

    // intakeOverAmbient: intake manifold / atmospheric pressure.
    Breakdown fmep(double rpm, double intakeOverAmbient) const;

    // Friction torque (N m) of the whole engine at this operating point.
    double torque(double rpm, double intakeOverAmbient) const;

private:
    Parameters m_parameters;
};

#endif /* ATG_ENGINE_SIM_ENGINE_FRICTION_MODEL_H */
