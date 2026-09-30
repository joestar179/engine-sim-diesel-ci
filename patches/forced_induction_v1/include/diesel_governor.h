#ifndef ATG_ENGINE_SIM_DIESEL_GOVERNOR_H
#define ATG_ENGINE_SIM_DIESEL_GOVERNOR_H

#include "throttle.h"
#include "fuel_rack_governor_model.h"

class DieselGovernor : public Throttle {
public:
    using Parameters = FuelRackGovernorModel::Parameters;

    DieselGovernor();
    virtual ~DieselGovernor();

    void initialize(const Parameters &parameters);
    virtual void setSpeedControl(double s) override;
    virtual void update(double dt, Engine *engine) override;

    double getTargetSpeed() const { return m_model.targetSpeed(); }
    double getFuelRack() const { return m_model.rack(); }
    bool isAirLimited() const { return m_airLimited; }

private:
    FuelRackGovernorModel m_model;
    bool m_airLimited = false;
};

#endif
