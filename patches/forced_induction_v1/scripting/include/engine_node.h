#ifndef ATG_ENGINE_SIM_ENGINE_NODE_H
#define ATG_ENGINE_SIM_ENGINE_NODE_H

#include "object_reference_node.h"

#include "crankshaft_node.h"
#include "cylinder_bank_node.h"
#include "ignition_module_node.h"
#include "engine_context.h"
#include "fuel_node.h"
#include "throttle_nodes.h"

#include "engine_sim.h"

#include <map>
#include <set>

namespace es_script {

    class EngineNode : public ObjectReferenceNode<EngineNode> {
    public:
        EngineNode() { /* void */ }
        virtual ~EngineNode() { /* void */ }

        void buildEngine(Engine *engine) {
            int cylinderCount = 0;
            for (const CylinderBankNode *bank : m_cylinderBanks) {
                cylinderCount += bank->getCylinderCount();
            }

            std::set<ExhaustSystemNode *> exhaustSystems;
            std::set<IntakeNode *> intakes;
            for (const CylinderBankNode *bank : m_cylinderBanks) {
                const int n = bank->getCylinderCount();
                for (int i = 0; i < n; ++i) {
                    exhaustSystems.insert(bank->getCylinder(i).exhaust);
                    intakes.insert(bank->getCylinder(i).intake);
                }
            }

            EngineContext context;
            context.setEngine(engine);

            Engine::Parameters parameters = m_parameters;
            parameters.crankshaftCount = (int)m_crankshafts.size();
            parameters.cylinderBanks = (int)m_cylinderBanks.size();
            parameters.cylinderCount = cylinderCount;
            parameters.exhaustSystemCount = (int)exhaustSystems.size();
            parameters.intakeCount = (int)intakes.size();
            parameters.throttle = m_throttle->generate();

            // The legacy script adapter still creates one TurboGroup, but
            // each script exhaust object may now select its physical inlet
            // channel. This exposes pulse grouping without coupling the core
            // model to ALCO, cylinder count, or a particular scroll count.
            if (parameters.turbocharger.enabled
                && parameters.forcedInduction.groups.empty()) {
                parameters.turbocharger.exhaustSystemIndices.clear();
                parameters.turbocharger.exhaustScrollIndices.clear();
                int exhaustIndex = 0;
                for (const ExhaustSystemNode *exhaust : exhaustSystems) {
                    parameters.turbocharger.exhaustSystemIndices.push_back(exhaustIndex++);
                    const int configuredScroll = exhaust->getTurboScrollIndex();
                    parameters.turbocharger.exhaustScrollIndices.push_back(
                        configuredScroll >= 0 ? configuredScroll : 0);
                }
                if (!parameters.turbocharger.exhaustSystemIndices.empty()) {
                    parameters.turbocharger.postTurbineExhaustIndex = 0;
                }
            }
            if (m_injectionRateProfile != nullptr
                && parameters.compressionIgnition.injectionProfileDuration > 0.0)
            {
                parameters.compressionIgnition.injectionRateProfile =
                    m_injectionRateProfile->generate(&context);
            }
            engine->initialize(parameters);

            {
                int i = 0;
                for (ExhaustSystemNode *exhaust : exhaustSystems) {
                    context.addExhaust(
                        exhaust, engine->getExhaustSystem(i++));
                }
            }

            {
                int i = 0;
                for (IntakeNode *intake : intakes) {
                    context.addIntake(
                        intake, engine->getIntake(i++));
                }
            }

            {
                int i = 0;
                for (const CylinderBankNode *bank : m_cylinderBanks) {
                    context.addHead(bank->getCylinderHead(), engine->getHead(i++));
                }
            }

            for (const CylinderBankNode *bank : m_cylinderBanks) {
                const int n = bank->getCylinderCount();
                for (int i = 0; i < n; ++i) {
                    exhaustSystems.insert(bank->getCylinder(i).exhaust);
                    intakes.insert(bank->getCylinder(i).intake);
                }
            }

            for (int i = 0; i < parameters.crankshaftCount; ++i) {
                m_crankshafts[i]->generate(engine->getCrankshaft(i), &context);
            }

            for (int i = 0; i < parameters.cylinderBanks; ++i) {
                m_cylinderBanks[i]->indexSlaveJournals(&context);
            }

            int cylinderIndex = 0;
            for (int i = 0; i < parameters.cylinderBanks; ++i) {
                m_cylinderBanks[i]->generate(
                    i,
                    cylinderIndex,
                    engine->getCylinderBank(i),
                    engine->getCrankshaft(0),
                    engine,
                    &context);
                cylinderIndex += m_cylinderBanks[i]->getCylinderCount();
            }

            for (int i = 0; i < parameters.cylinderBanks; ++i) {
                m_cylinderBanks[i]->connectRodAssemblies(&context);
            }

            m_ignitionModule->generate(engine, &context);
            {
                // Component friction: engine geometry from the built engine,
                // bearing and valvetrain geometry from the script (m -> mm).
                EngineFrictionModel::Parameters f = m_frictionParameters;
                f.cylinders = engine->getCylinderCount();
                if (f.enabled && engine->getCylinderBankCount() > 0 && engine->getCrankshaftCount() > 0) {
                    const double bore = engine->getCylinderBank(0)->getBore();
                    const double stroke = 2.0 * engine->getCrankshaft(0)->getThrow();
                    const double swept = constants::pi / 4.0 * bore * bore * stroke;
                    const double clearance = engine->getHead(0)->getCombustionChamberVolume();
                    f.bore = bore * 1000.0;
                    f.stroke = stroke * 1000.0;
                    f.compressionRatio = clearance > 0.0 ? (swept + clearance) / clearance : 0.0;
                    f.mainBearingDiameter *= 1000.0;
                    f.mainBearingLength *= 1000.0;
                    f.rodBearingDiameter *= 1000.0;
                    f.rodBearingLength *= 1000.0;
                    f.maxValveLift *= 1000.0;
                }
                engine->initializeFrictionModel(f);
            }
            if (m_fuelStopCurve != nullptr) {
                engine->setFuelStopCurve(m_fuelStopCurve->generate(&context));
            }
            engine->configureIntakesForCombustionMode();
            engine->configureForcedInductionGasPath();
            
            Function *meanPistonSpeedToTurbulence = new Function;
            meanPistonSpeedToTurbulence->initialize(30, 1);
            for (int i = 0; i < 30; ++i) {
                const double s = (double)i;
                meanPistonSpeedToTurbulence->addSample(s, s * 0.5);
            }

            Fuel *fuel = engine->getFuel();
            m_fuel->generate(fuel, &context);

            CombustionChamber::Parameters ccParams;
            ccParams.CrankcasePressure = units::pressure(1.0, units::atm);
            ccParams.Fuel = fuel;
            ccParams.StartingPressure = units::pressure(1.0, units::atm);
            ccParams.StartingTemperature = units::celcius(25.0);
            ccParams.MeanPistonSpeedToTurbulence = meanPistonSpeedToTurbulence;
            ccParams.ChamberAreaRatio = m_chamberAreaRatio;
            ccParams.PistonWallTemperature = m_pistonWallTemperature;
            ccParams.HeadWallTemperature = m_headWallTemperature;
            ccParams.LinerWallTemperature = m_linerWallTemperature;

            for (int i = 0; i < engine->getCylinderCount(); ++i) {
                ccParams.Piston = engine->getPiston(i);
                ccParams.Head = engine->getHead(ccParams.Piston->getCylinderBank()->getIndex());
                engine->getChamber(i)->initialize(ccParams);
            }
        }

        void addCrankshaft(CrankshaftNode *crankshaft) {
            m_crankshafts.push_back(crankshaft);
        }

        void addCylinderBank(CylinderBankNode *bank) {
            m_cylinderBanks.push_back(bank);
        }

        int getIgnitionModuleCount() const {
            return m_ignitionModule == nullptr
                ? 0
                : 1;
        }

        void addIgnitionModule(IgnitionModuleNode *ignitionModule) {
            m_ignitionModule = ignitionModule;
        }

    protected:
        virtual void registerInputs() {
            addInput("name", &m_parameters.name);
            addInput("starter_torque", &m_parameters.starterTorque);
            addInput("starter_speed", &m_parameters.starterSpeed);
            addInput("dyno_min_speed", &m_parameters.dynoMinSpeed);
            addInput("dyno_max_speed", &m_parameters.dynoMaxSpeed);
            addInput("dyno_hold_step", &m_parameters.dynoHoldStep);
            addInput("redline", &m_parameters.redline);
            addInput("compression_ignition", &m_parameters.compressionIgnition.enabled);
            addInput("max_fuel_mass_per_cycle", &m_parameters.compressionIgnition.maxFuelMassPerCycle);
            addInput("fuel_stop_curve", &m_fuelStopCurve);
            addInput("injection_rate_profile", &m_injectionRateProfile);
            addInput("injection_profile_duration", &m_parameters.compressionIgnition.injectionProfileDuration);
            addInput("component_friction", &m_frictionParameters.enabled);
            addInput("main_bearing_count", &m_frictionParameters.mainBearings);
            addInput("main_bearing_diameter", &m_frictionParameters.mainBearingDiameter);
            addInput("main_bearing_length", &m_frictionParameters.mainBearingLength);
            addInput("rod_bearing_diameter", &m_frictionParameters.rodBearingDiameter);
            addInput("rod_bearing_length", &m_frictionParameters.rodBearingLength);
            addInput("cam_bearing_count", &m_frictionParameters.camBearings);
            addInput("valve_count", &m_frictionParameters.valves);
            addInput("max_valve_lift", &m_frictionParameters.maxValveLift);
            addInput("valvetrain_flat_follower", &m_frictionParameters.flatFollower);
            addInput("valvetrain_roller_follower", &m_frictionParameters.rollerFollower);
            addInput("valvetrain_oscillating_hydrodynamic", &m_frictionParameters.oscillatingHydrodynamic);
            addInput("valvetrain_oscillating_mixed", &m_frictionParameters.oscillatingMixed);
            addInput("oil_viscosity_ratio", &m_frictionParameters.viscosityRatio);
            addInput("chamber_area_ratio", &m_chamberAreaRatio);
            addInput("piston_wall_temperature", &m_pistonWallTemperature);
            addInput("head_wall_temperature", &m_headWallTemperature);
            addInput("liner_wall_temperature", &m_linerWallTemperature);
            addInput("injection_duration", &m_parameters.compressionIgnition.injectionDuration);
            addInput("ignition_delay", &m_parameters.compressionIgnition.ignitionDelay);
            addInput("combustion_duration", &m_parameters.compressionIgnition.combustionDuration);
            addInput("premixed_burn_fraction", &m_parameters.compressionIgnition.premixedBurnFraction);
            addInput("autoignition_temperature", &m_parameters.compressionIgnition.autoignitionTemperature);
            addInput("autoignition_pressure", &m_parameters.compressionIgnition.autoignitionPressure);
            addInput("ignition_delay_correlation", &m_parameters.compressionIgnition.ignitionDelayCorrelation);
            addInput("nozzle_hole_count", &m_parameters.compressionIgnition.nozzleHoles);
            addInput("nozzle_hole_diameter", &m_parameters.compressionIgnition.nozzleHoleDiameter);
            addInput("injection_pressure", &m_parameters.compressionIgnition.injectionPressure);
            addInput("nozzle_discharge_coefficient", &m_parameters.compressionIgnition.nozzleDischargeCoefficient);
            addInput("pump_plunger_count", &m_parameters.compressionIgnition.pumpPlungers);
            addInput("pump_plunger_diameter", &m_parameters.compressionIgnition.pumpPlungerDiameter);
            addInput("pump_cam_lift_rate", &m_parameters.compressionIgnition.pumpCamLiftRate);
            addInput("turbo_enabled", &m_parameters.turbocharger.enabled);
            addInput("turbo_shaft_inertia", &m_parameters.turbocharger.shaftInertia);
            addInput("turbo_friction_torque", &m_parameters.turbocharger.frictionTorque);
            addInput("turbo_max_speed", &m_parameters.turbocharger.maxSpeed);
            addInput("turbo_max_pressure_ratio", &m_parameters.turbocharger.maxPressureRatio);
            addInput("compressor_efficiency", &m_parameters.turbocharger.compressorEfficiency);
            addInput("turbine_efficiency", &m_parameters.turbocharger.turbineEfficiency);
            addInput("aftercooler_effectiveness", &m_parameters.turbocharger.aftercoolerEffectiveness);
            addInput("turbo_design_mass_flow", &m_parameters.turbocharger.designMassFlow);
            addInput("turbine_design_pressure_ratio", &m_parameters.turbocharger.turbineDesignPressureRatio);
            addInput("turbine_design_temperature", &m_parameters.turbocharger.turbineDesignTemperature);
            addInput("turbo_pre_turbine_volume", &m_parameters.turbocharger.preTurbineVolume);
            addInput("turbo_pre_turbine_area", &m_parameters.turbocharger.preTurbineArea);
            addInput("turbo_inlet_channel_count", &m_parameters.turbocharger.inletChannelCount);
            addInput("turbo_compressor_inlet_volume", &m_parameters.turbocharger.compressorInletVolume);
            addInput("turbo_compressor_discharge_volume", &m_parameters.turbocharger.compressorDischargeVolume);
            addInput("turbo_cooler_volume", &m_parameters.turbocharger.coolerVolume);
            addInput("turbo_charge_plenum_volume", &m_parameters.turbocharger.chargePlenumVolume);
            addInput("turbo_charge_area", &m_parameters.turbocharger.chargeArea);
            addInput("turbo_inlet_flow_rate", &m_parameters.turbocharger.inletFlowRate);
            addInput("turbo_passive_compressor_flow_rate", &m_parameters.turbocharger.passiveCompressorFlowRate);
            addInput("turbo_cooler_flow_rate", &m_parameters.turbocharger.coolerFlowRate);
            addInput("turbo_charge_flow_rate", &m_parameters.turbocharger.chargeFlowRate);
            addInput("turbo_turbine_flow_rate", &m_parameters.turbocharger.turbineFlowRate);
            addInput("aftercooler_enabled", &m_parameters.turbocharger.chargeAirCoolerEnabled);
            addInput("aftercooler_pressure_loss", &m_parameters.turbocharger.coolerPressureLoss);
            addInput("turbo_throttle_enabled", &m_parameters.turbocharger.throttleEnabled);
            addInput("wastegate_enabled", &m_parameters.turbocharger.wastegateEnabled);
            addInput("wastegate_flow_rate", &m_parameters.turbocharger.wastegateFlowRate);
            addInput("wastegate_position", &m_parameters.turbocharger.wastegatePosition);
            addInput("wastegate_time_constant", &m_parameters.turbocharger.wastegateTimeConstant);
            addInput("compressor_bypass_enabled", &m_parameters.turbocharger.compressorBypassEnabled);
            addInput("compressor_bypass_flow_rate", &m_parameters.turbocharger.compressorBypassFlowRate);
            addInput("compressor_bypass_position", &m_parameters.turbocharger.compressorBypassPosition);
            addInput("compressor_bypass_time_constant", &m_parameters.turbocharger.compressorBypassTimeConstant);
            addInput("compressor_bypass_recirculates", &m_parameters.turbocharger.compressorBypassRecirculates);
            addInput("vgt_enabled", &m_parameters.turbocharger.vgtEnabled);
            addInput("vgt_position", &m_parameters.turbocharger.vgtPosition);
            addInput("vgt_min_flow_factor", &m_parameters.turbocharger.vgtMinFlowFactor);
            addInput("vgt_time_constant", &m_parameters.turbocharger.vgtTimeConstant);
            addInput("audio_low_speed_full_strength", &m_parameters.proceduralAudio.lowSpeedFullStrength);
            addInput("audio_low_speed_exponent", &m_parameters.proceduralAudio.lowSpeedExponent);
            addInput("combustion_audio_gain", &m_parameters.proceduralAudio.combustionGain);
            addInput("turbo_tone_audio_gain", &m_parameters.proceduralAudio.turboToneGain);
            addInput("turbo_noise_audio_gain", &m_parameters.proceduralAudio.turboNoiseGain);
            addInput("compressor_blade_count", &m_parameters.proceduralAudio.compressorBladeCount);
            addInput("fuel", &m_fuel, InputTarget::Type::Object);
            addInput("throttle", &m_throttle, InputTarget::Type::Object);
            addInput("simulation_frequency", &m_parameters.initialSimulationFrequency);
            addInput("hf_gain", &m_parameters.initialHighFrequencyGain);
            addInput("jitter", &m_parameters.initialJitter);
            addInput("noise", &m_parameters.initialNoise);

            ObjectReferenceNode<EngineNode>::registerInputs();
        }

        virtual void _evaluate() {
            setOutput(this);

            // Read inputs
            readAllInputs();
        }

        ThrottleNode *m_throttle = nullptr;
        IgnitionModuleNode *m_ignitionModule = nullptr;
        FunctionNode *m_fuelStopCurve = nullptr;
        FunctionNode *m_injectionRateProfile = nullptr;
        EngineFrictionModel::Parameters m_frictionParameters;
        double m_chamberAreaRatio = 1.0;
        double m_pistonWallTemperature = 573.0;
        double m_headWallTemperature = 503.0;
        double m_linerWallTemperature = 423.0;
        FuelNode *m_fuel = nullptr;

        Engine::Parameters m_parameters;
        std::vector<CrankshaftNode *> m_crankshafts;
        std::vector<CylinderBankNode *> m_cylinderBanks;
    };

} /* namespace es_script */

#endif /* ATG_ENGINE_SIM_ENGINE_NODE_H */
