#ifndef ATG_ENGINE_SIM_FORCED_INDUCTION_SYSTEM_H
#define ATG_ENGINE_SIM_FORCED_INDUCTION_SYSTEM_H

#include "gas_system.h"
#include "turbocharger_model.h"

#include <cstddef>
#include <vector>

class Engine;

// One TurboGroup owns one physical shaft, its compressor-side gas volumes,
// one or more pulse-separated turbine-inlet volumes, and its optional valves.
// Routing is index based so multiple independent groups can be attached to one
// engine without introducing an engine-global turbo assumption.
class TurboGroup {
public:
    struct Parameters {
        bool enabled = false;

        // Rotating assembly and reduced-order compressor/turbine maps.
        double shaftInertia = 1.0;
        double frictionTorque = 0.0;
        double maxSpeed = 1.0;
        double maxPressureRatio = 1.0;
        double compressorEfficiency = 0.70;
        double turbineEfficiency = 0.70;
        double designMassFlow = 1.0;
        double turbineDesignPressureRatio = 1.8;
        double turbineDesignTemperature = 800.0;

        // Gas-path geometry. Values are SI after script evaluation.
        double preTurbineVolume = 0.020;
        double preTurbineArea = 0.010;
        int inletChannelCount = 1;
        double compressorInletVolume = 0.020;
        double compressorDischargeVolume = 0.015;
        double coolerVolume = 0.020;
        double chargePlenumVolume = 0.050;
        double chargeArea = 0.040;

        // Optional calibrated design-flow capacities in kg/s. Zero derives a
        // stable capacity from designMassFlow; none imposes pressure or boost.
        double inletFlowRate = 0.0;
        double passiveCompressorFlowRate = 0.0;
        double coolerFlowRate = 0.0;
        double chargeFlowRate = 0.0;
        double turbineFlowRate = 0.0;

        bool chargeAirCoolerEnabled = false;
        double aftercoolerEffectiveness = 0.0;
        double coolerPressureLoss = 0.0;

        bool throttleEnabled = false;

        bool wastegateEnabled = false;
        double wastegateFlowRate = 0.0;
        double wastegatePosition = 0.0;
        double wastegateTimeConstant = 0.0;

        bool compressorBypassEnabled = false;
        double compressorBypassFlowRate = 0.0;
        double compressorBypassPosition = 0.0;
        double compressorBypassTimeConstant = 0.0;
        bool compressorBypassRecirculates = true;

        bool vgtEnabled = false;
        double vgtPosition = 1.0;
        double vgtMinFlowFactor = 0.25;
        double vgtTimeConstant = 0.0;

        // Empty routes mean "all" only for the single legacy-script adapter.
        // Native multi-group callers provide explicit, non-overlapping routes.
        std::vector<int> exhaustSystemIndices;
        std::vector<int> exhaustScrollIndices;
        std::vector<int> intakeIndices;
        int postTurbineExhaustIndex = -1;
    };

    struct Telemetry {
        std::vector<double> preTurbinePressure;
        std::vector<double> preTurbineTemperature;
        double turbineMassFlow = 0.0;
        double turbinePressureRatio = 1.0;
        double turbinePower = 0.0;
        double wastegateMassFlow = 0.0;
        double postTurbinePressure = 101325.0;
        double postTurbineTemperature = 298.15;
        double shaftSpeed = 0.0;
        double compressorMassFlow = 0.0;
        double compressorPressureRatio = 1.0;
        double compressorPower = 0.0;
        double compressorDischargeTemperature = 298.15;
        double coolerInletPressure = 101325.0;
        double coolerInletTemperature = 298.15;
        double coolerOutletPressure = 101325.0;
        double coolerOutletTemperature = 298.15;
        double chargePlenumPressure = 101325.0;
        double chargePlenumTemperature = 298.15;
        double bypassMassFlow = 0.0;
        double intakeMassFlow = 0.0;
        double wastegatePosition = 0.0;
        double compressorBypassPosition = 0.0;
        double vgtPosition = 1.0;
    };

public:
    TurboGroup();

    void initialize(const Parameters &parameters);
    bool enabled() const { return m_parameters.enabled; }
    const Parameters &parameters() const { return m_parameters; }
    const Telemetry &telemetry() const { return m_telemetry; }

    GasSystem *preTurbineSystem(int scrollIndex);
    const GasSystem *preTurbineSystem(int scrollIndex) const;
    TurbochargerModel *rotatingAssembly() { return &m_rotatingAssembly; }
    const TurbochargerModel *rotatingAssembly() const { return &m_rotatingAssembly; }

    // Generic controller boundary. These commands only move physical
    // actuators; they never assign boost pressure, airflow or shaft speed.
    void setWastegateCommand(double command);
    void setCompressorBypassCommand(double command);
    void setVgtCommand(double command);

    void process(double dt, Engine &engine);

private:
    double flowCoefficient(double configuredMassFlow, double designPressure,
        double designPressureDrop, double designTemperature) const;
    double updateActuator(double current, double command, double timeConstant, double dt) const;
    double transferCompressedGas(double dt);
    double processTurbineFlow(double dt, Engine &engine);
    double processWastegateFlow(double dt, Engine &engine);
    double processChargePath(double dt, Engine &engine);
    double processBypass(double dt);
    void updateTelemetry(Engine &engine);

private:
    Parameters m_parameters;
    Telemetry m_telemetry;
    TurbochargerModel m_rotatingAssembly;

    GasSystem m_ambient;
    GasSystem m_compressorInlet;
    GasSystem m_compressorDischarge;
    GasSystem m_cooler;
    GasSystem m_chargePlenum;
    std::vector<GasSystem> m_preTurbine;

    double m_inletFlowK = 0.0;
    double m_passiveCompressorFlowK = 0.0;
    double m_coolerFlowK = 0.0;
    double m_chargeFlowK = 0.0;
    double m_turbineFlowK = 0.0;
    double m_wastegateFlowK = 0.0;
    double m_bypassFlowK = 0.0;

    double m_wastegateState = 0.0;
    double m_bypassState = 0.0;
    double m_vgtState = 1.0;
    double m_wastegateCommand = 0.0;
    double m_bypassCommand = 0.0;
    double m_vgtCommand = 1.0;
};

class ForcedInductionSystem {
public:
    struct Parameters {
        std::vector<TurboGroup::Parameters> groups;
    };

public:
    void initialize(const Parameters &parameters, int exhaustSystemCount, int intakeCount);
    bool enabled() const { return !m_groups.empty(); }
    std::size_t groupCount() const { return m_groups.size(); }
    TurboGroup *group(std::size_t index);
    const TurboGroup *group(std::size_t index) const;

    bool managesIntake(int intakeIndex) const;
    GasSystem *exhaustDestination(int exhaustSystemIndex, GasSystem *naturallyAspiratedDestination);
    void process(double dt, Engine &engine);

private:
    struct ExhaustRoute {
        int group = -1;
        int scroll = -1;
    };

    std::vector<TurboGroup> m_groups;
    std::vector<ExhaustRoute> m_exhaustRoutes;
    std::vector<int> m_intakeRoutes;
};

#endif
