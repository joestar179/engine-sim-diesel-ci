#ifndef ATG_ENGINE_SIM_ENGINE_H
#define ATG_ENGINE_SIM_ENGINE_H

#include "part.h"

#include "piston.h"
#include "connecting_rod.h"
#include "crankshaft.h"
#include "cylinder_bank.h"
#include "cylinder_head.h"
#include "exhaust_system.h"
#include "ignition_module.h"
#include "intake.h"
#include "combustion_chamber.h"
#include "units.h"
#include "throttle.h"
#include "compression_ignition_model.h"
#include "forced_induction_system.h"
#include "procedural_diesel_audio.h"

#include <string>

class Simulator;
class Vehicle;
class Transmission;
class Engine : public Part {
    public:
        struct Parameters {
            int cylinderBanks;
            int cylinderCount;
            int crankshaftCount;
            int exhaustSystemCount;
            int intakeCount;

            std::string name;

            double starterTorque = units::torque(90.0, units::ft_lb);
            double starterSpeed = units::rpm(200);
            double redline = units::rpm(6500);
            double dynoMinSpeed = units::rpm(1000);
            double dynoMaxSpeed = units::rpm(6500);
            double dynoHoldStep = units::rpm(100);

            CompressionIgnitionModel::Parameters compressionIgnition;
            // Backward-compatible one-group script adapter. Native C++ users
            // can provide any number of explicitly routed groups below.
            TurboGroup::Parameters turbocharger;
            ForcedInductionSystem::Parameters forcedInduction;
            ProceduralDieselAudio::Parameters proceduralAudio;

            Throttle *throttle;

            double initialSimulationFrequency;
            double initialHighFrequencyGain;
            double initialNoise;
            double initialJitter;
        };

    public:
        Engine();
        virtual ~Engine();

        void initialize(const Parameters &params);
        virtual void destroy();

        std::string getName() const { return m_name; }

        virtual Crankshaft *getOutputCrankshaft() const;
        virtual void setSpeedControl(double s);
        virtual double getSpeedControl();
        virtual void setThrottle(double throttle);
        void setFuelRack(double rack);
        double getFuelRack() const { return m_fuelRack; }
        double getFuelMassPerCycleCommand() const;
        virtual double getThrottle() const;
        virtual double getThrottlePlateAngle() const;
        virtual void calculateDisplacement();
        double getDisplacement() const { return m_displacement; }
        virtual double getIntakeFlowRate() const;
        virtual void update(double dt);
        void configureIntakesForCombustionMode();
        void configureForcedInductionGasPath();
        GasSystem *getExhaustDestination(ExhaustSystem *exhaust);
        void processForcedInduction(double dt);

        virtual double getManifoldPressure() const;
        virtual double getIntakeAfr() const;
        virtual double getExhaustO2() const;
        virtual double getRpm() const;
        virtual double getSpeed() const;
        virtual bool isSpinningCw() const;

        virtual void resetFuelConsumption();
        virtual double getTotalFuelMassConsumed() const;
        double getTotalVolumeFuelConsumed() const;

        inline double getStarterTorque() const { return m_starterTorque; }
        inline double getStarterSpeed() const { return m_starterSpeed; }
        inline double getRedline() const { return m_redline; }
        inline double getDynoMinSpeed() const { return m_dynoMinSpeed; }
        inline double getDynoMaxSpeed() const { return m_dynoMaxSpeed; }
        inline double getDynoHoldStep() const { return m_dynoHoldStep; }

        int getCylinderBankCount() const { return m_cylinderBankCount; }
        int getCylinderCount() const { return m_cylinderCount; }
        int getCrankshaftCount() const { return m_crankshaftCount; }
        int getExhaustSystemCount() const { return m_exhaustSystemCount; }
        int getIntakeCount() const { return m_intakeCount; }
        int getMaxDepth() const;

        Crankshaft *getCrankshaft(int i) const { return &m_crankshafts[i]; }
        CylinderBank *getCylinderBank(int i) const { return &m_cylinderBanks[i]; }
        CylinderHead *getHead(int i) const { return &m_heads[i]; }
        Piston *getPiston(int i) const { return &m_pistons[i]; }
        ConnectingRod *getConnectingRod(int i) const { return &m_connectingRods[i]; }
        IgnitionModule *getIgnitionModule() { return &m_ignitionModule; }
        ExhaustSystem *getExhaustSystem(int i) const { return &m_exhaustSystems[i]; }
        Intake *getIntake(int i) const { return &m_intakes[i]; }
        CombustionChamber *getChamber(int i) const { return &m_combustionChambers[i]; }
        Fuel *getFuel() { return &m_fuel; }
        bool isCompressionIgnition() const { return m_compressionIgnition.enabled(); }
        CompressionIgnitionModel *getCompressionIgnitionModel() { return &m_compressionIgnition; }
        ForcedInductionSystem *getForcedInductionSystem() { return &m_forcedInduction; }
        const ForcedInductionSystem *getForcedInductionSystem() const { return &m_forcedInduction; }
        TurbochargerModel *getTurbochargerModel();
        ProceduralDieselAudio *getProceduralDieselAudio() { return &m_proceduralAudio; }
        double getTurboSpeed() const;
        double getCompressorPressureRatio() const;
        double getCompressorPower() const;
        double getTurbinePower() const;
        double getTurbinePressureRatio() const;
        double getTurbineInletPressure() const;
        double getTurbineOutletPressure() const;
        void recordDirectInjectedFuelMass(double mass) { m_directInjectedFuelMass += mass; }
        void recordDirectBurnedFuelMass(double mass) { m_directBurnedFuelMass += mass; }
        double getDirectInjectedFuelMass() const { return m_directInjectedFuelMass; }
        double getDirectBurnedFuelMass() const { return m_directBurnedFuelMass; }
        void recordCompressionIgnitionConditions(double temperature, double pressure) { if (temperature > m_maxCiTemperature) m_maxCiTemperature = temperature; if (pressure > m_maxCiPressure) m_maxCiPressure = pressure; }
        double getMaxCompressionIgnitionTemperature() const { return m_maxCiTemperature; }
        double getMaxCompressionIgnitionPressure() const { return m_maxCiPressure; }

        double getSimulationFrequency() const { return m_initialSimulationFrequency; }
        double getInitialHighFrequencyGain() const { return m_initialHighFrequencyGain; }
        double getInitialNoise() const { return m_initialNoise; }
        double getInitialJitter() const { return m_initialJitter; }

        virtual Simulator *createSimulator(Vehicle *vehicle, Transmission *transmission);

    protected:
        std::string m_name;

        Crankshaft *m_crankshafts;
        int m_crankshaftCount;

        CylinderBank *m_cylinderBanks;
        CylinderHead *m_heads;
        int m_cylinderBankCount;

        Piston *m_pistons;
        ConnectingRod *m_connectingRods;
        CombustionChamber *m_combustionChambers;
        int m_cylinderCount;

        double m_starterTorque;
        double m_starterSpeed;
        double m_redline;
        double m_dynoMinSpeed;
        double m_dynoMaxSpeed;
        double m_dynoHoldStep;

        double m_initialSimulationFrequency;
        double m_initialHighFrequencyGain;
        double m_initialNoise;
        double m_initialJitter;

        ExhaustSystem *m_exhaustSystems;
        int m_exhaustSystemCount;

        Intake *m_intakes;
        int m_intakeCount;

        IgnitionModule m_ignitionModule;
        Fuel m_fuel;

        CompressionIgnitionModel m_compressionIgnition;
        ForcedInductionSystem m_forcedInduction;
        TurbochargerModel m_disabledTurbocharger;
        ProceduralDieselAudio m_proceduralAudio;
        double m_fuelRack = 0.0;
        double m_directInjectedFuelMass = 0.0;
        double m_directBurnedFuelMass = 0.0;
        double m_maxCiTemperature = 0.0;
        double m_maxCiPressure = 0.0;

        Throttle *m_throttle;

        double m_throttleValue;
        double m_displacement;
};

#endif /* ATG_ENGINE_SIM_ENGINE_H */

