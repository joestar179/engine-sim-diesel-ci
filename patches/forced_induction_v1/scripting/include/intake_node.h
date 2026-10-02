#ifndef ATG_ENGINE_SIM_INTAKE_NODE_H
#define ATG_ENGINE_SIM_INTAKE_NODE_H

#include "object_reference_node.h"

#include "engine_context.h"
#include "function_node.h"

#include "engine_sim.h"

#include <map>
#include <vector>

namespace es_script {

    class IntakeNode : public ObjectReferenceNode<IntakeNode> {
    public:
        IntakeNode() { /* void */ }
        virtual ~IntakeNode() { /* void */ }

        Intake *generate(EngineContext *context) {
            Intake *intake = context->getIntake(this);
            Intake::Parameters parameters = m_parameters;
            // Ambient (supply) state of the intake; <= 0 keeps the built-in
            // 1 atm / 25 C.
            if (m_atmospherePressure > 0.0) parameters.AtmospherePressure = m_atmospherePressure;
            if (m_atmosphereTemperature > 0.0) parameters.AtmosphereTemperature = m_atmosphereTemperature;
            intake->initialize(parameters);

            return intake;
        }

    protected:
        virtual void registerInputs() {
            addInput("plenum_volume", &m_parameters.volume);
            addInput("plenum_cross_section_area", &m_parameters.CrossSectionArea);
            addInput("intake_flow_rate", &m_parameters.InputFlowK);
            addInput("idle_flow_rate", &m_parameters.IdleFlowK);
            addInput("runner_flow_rate", &m_parameters.RunnerFlowRate);
            addInput("molecular_afr", &m_parameters.MolecularAfr);
            addInput("idle_throttle_plate_position", &m_parameters.IdleThrottlePlatePosition);
            addInput("throttle_gamma", &m_throttleGammaUnused);
            addInput("runner_length", &m_parameters.RunnerLength);
            addInput("velocity_decay", &m_parameters.VelocityDecay);
            addInput("oxygen_fraction", &m_parameters.OxygenFraction);
            addInput("products_fraction", &m_parameters.ProductsFraction);
            addInput("atmosphere_pressure", &m_atmospherePressure);
            addInput("atmosphere_temperature", &m_atmosphereTemperature);

            ObjectReferenceNode<IntakeNode>::registerInputs();
        }

        virtual void _evaluate() {
            setOutput(this);

            // Read inputs
            readAllInputs();
        }

        double m_throttleGammaUnused = 0.0; // Deprecated; to be removed in a future release
        double m_atmospherePressure = -1.0;
        double m_atmosphereTemperature = -1.0;
        Intake::Parameters m_parameters;
    };

} /* namespace es_script */

#endif /* ATG_ENGINE_SIM_INTAKE_NODE_H */
