#ifndef ATG_ENGINE_SIM_INTAKE_H
#define ATG_ENGINE_SIM_INTAKE_H

#include "part.h"

#include "gas_system.h"

class Engine;

class Intake : public Part {
    public:
        struct Parameters {
            // Plenum volume
            double volume;

            // Plenum dimensions
            double CrossSectionArea;

            // Input flow constant
            double InputFlowK;

            // Idle-circuit flow constant
            double IdleFlowK;

            // Flow rate from plenum to runner
            double RunnerFlowRate;

            // Molecular air fuel ratio (defaults to ideal for octane)
            double MolecularAfr = (25.0 / 2.0);

            // Throttle plate position at idle
            double IdleThrottlePlatePosition = 0.975;

            // Runner volume
            double RunnerLength = units::distance(4.0, units::inch);

            // Velocity decay factor
            double VelocityDecay = 0.5;

            bool AirOnly = false;
            double AtmospherePressure = units::pressure(1.0, units::atm);
            double AtmosphereTemperature = units::celcius(25.0);

            // Intake charge composition (mole fractions of the air part).
            // OxygenFraction < 0 keeps the legacy composition (20.95 % O2 for
            // air-only intakes, the stock 25 % for premixed intakes).
            // ProductsFraction is CO2 + H2O (EGR / synthetic dilution); the
            // remainder is N2.
            double OxygenFraction = -1.0;
            double ProductsFraction = 0.0;
        };

    public:
        Intake();
        virtual ~Intake();

        void initialize(Parameters &params);
        virtual void destroy();

        void process(double dt);

        inline double getRunnerFlowRate() const { return m_runnerFlowRate; }
        inline double getThrottlePlatePosition() const { return m_idleThrottlePlatePosition * m_throttle; }
        inline double getRunnerLength() const { return m_runnerLength; }
        inline double getPlenumCrossSectionArea() const { return m_crossSectionArea; }
        inline double getVelocityDecay() const { return m_velocityDecay; }
        GasSystem *getSystem() { return &m_system; }
        void setEngine(Engine *engine) { m_engine = engine; }
        void setAirOnly(bool airOnly) { m_airOnly = airOnly; }
        bool isAirOnly() const { return m_airOnly; }
        void setForcedInductionFeed(bool enabled) { m_forcedInductionFeed = enabled; }
        bool hasForcedInductionFeed() const { return m_forcedInductionFeed; }
        void recordForcedInductionAir(double airMoles);

        GasSystem m_system;
        double m_throttle;

        double m_flow;
        double m_flowRate;
        double m_totalFuelInjected;

    protected:
        double m_crossSectionArea;
        double m_inputFlowK;
        double m_idleFlowK;
        double m_runnerFlowRate;
        double m_molecularAfr;
        double m_idleThrottlePlatePosition;
        double m_runnerLength;
        double m_velocityDecay;
        bool m_airOnly = false;
        double m_atmospherePressure = units::pressure(1.0, units::atm);
        double m_atmosphereTemperature = units::celcius(25.0);
        double m_oxygenFraction = -1.0;
        double m_productsFraction = 0.0;
        bool m_forcedInductionFeed = false;
        Engine *m_engine = nullptr;

        GasSystem m_atmosphere;
};

#endif /* ATG_ENGINE_SIM_INTAKE_H */
