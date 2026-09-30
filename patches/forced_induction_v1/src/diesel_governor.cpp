#include "../include/diesel_governor.h"
#include "../include/engine.h"

DieselGovernor::DieselGovernor() = default;
DieselGovernor::~DieselGovernor() = default;

void DieselGovernor::initialize(const Parameters &parameters) {
    m_model.initialize(parameters);
}

void DieselGovernor::setSpeedControl(double s) {
    Throttle::setSpeedControl(s);
    m_model.setSpeedControl(s);
}

void DieselGovernor::update(double dt, Engine *engine) {
    double rack = m_model.update(dt, engine->getSpeed());

    // Air/smoke limiter: cap the fuel to what the charge actually trapped in
    // the cylinders can burn at the configured minimum excess-air ratio.
    // It reads the physical air state (oxygen trapped at each cylinder's
    // latest injection), so a turbocharged engine is allowed more fuel as
    // boost builds and a naturally aspirated one is held back.
    const double lambda = m_model.parameters().smokeLimitLambda;
    m_airLimited = false;
    if (lambda > 0.0 && engine->isCompressionIgnition()) {
        double trappedO2 = 0.0;
        int samples = 0;
        for (int i = 0; i < engine->getCylinderCount(); ++i) {
            const double o2 = engine->getChamber(i)->getLastInjectionTrappedO2Moles();
            if (o2 > 0.0) {
                trappedO2 += o2;
                ++samples;
            }
        }
        const double maxFuelMass = engine->getCompressionIgnitionModel()->parameters().maxFuelMassPerCycle;
        Fuel *fuel = engine->getFuel();
        if (samples > 0 && maxFuelMass > 0.0 && fuel->getMolecularAfr() > 0.0) {
            const double fuelMoles = (trappedO2 / samples) / (lambda * fuel->getMolecularAfr());
            const double rackLimit = fuelMoles * fuel->getMolecularMass() / maxFuelMass;
            if (rack > rackLimit) {
                rack = rackLimit;
                m_airLimited = true;
            }
        }
    }

    engine->setFuelRack(rack);
}
