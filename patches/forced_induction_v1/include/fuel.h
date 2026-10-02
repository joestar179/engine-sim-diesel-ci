#ifndef ATG_ENGINE_FUEL_H
#define ATG_ENGINE_FUEL_H

#include "units.h"

#include "function.h"

#include <string>

class Fuel {
    public:
        struct Parameters {
            std::string name = "Gasoline";
            double molecularMass =
                units::mass(100.0, units::g);
            double energyDensity =
                units::energy(48.1, units::kJ) / units::mass(1.0, units::g);
            double density =
                units::mass(0.755, units::kg) / units::volume(1.0, units::L);
            double molecularAfr = 25 / 2.0;
            double burningEfficiencyRandomness = 0.5;
            double lowEfficiencyAttenuation = 0.6;
            double maxBurningEfficiency = 0.8;
            double maxTurbulenceEffect = 2.0;
            double maxDilutionEffect = 50.0;
            Function *turbulenceToFlameSpeedRatio = nullptr;

            // Laminar burning velocity S_L = (B_m + B_x (x - x_m)^2)
            // (T/298)^alpha (p/1 atm)^beta, alpha = a0 + a1 x^a2,
            // beta = b0 + b1 x^b2, x = mixture ratio as computed by the
            // chamber (currently lambda; see FAILURE_ATTEMPT_LOG). Defaults:
            // the stock gasoline coefficients.
            double laminarPeakMixture = 1.21;
            double laminarPeakSpeed = units::distance(30.5, units::cm) / units::sec;
            double laminarSpeedCurvature = -units::distance(54.9, units::cm) / units::sec;
            double laminarAlpha0 = 2.4;
            double laminarAlpha1 = -0.271;
            double laminarAlpha2 = 3.51;
            double laminarBeta0 = -0.357;
            double laminarBeta1 = 0.14;
            double laminarBeta2 = 2.77;
        };

        Fuel();
        ~Fuel();

        void initialize(const Parameters &params);

        inline double getMolecularMass() const { return m_molecularMass; }
        inline double getEnergyDensity() const { return m_energyDensity; }
        inline double getDensity() const { return m_density; }
        inline double getBurningEfficiencyRandomness() const { return m_burningEfficiencyRandomness; }
        inline double getLowEfficiencyAttenuation() const { return m_lowEfficiencyAttenuation;  }
        inline double getMaxBurningEfficiency() const { return m_maxBurningEfficiency; }
        inline double getMaxTurbulenceEffect() const { return m_maxTurbulenceEffect; }
        inline double getMaxDilutionEffect() const { return m_maxDilutionEffect; }

        double flameSpeed(
            double turbulence,
            double molecularAfr,
            double T,
            double P,
            double firingPressure,
            double motoringPressure) const;
        virtual double laminarBurningVelocity(double molecularAfr, double T, double P) const;

        double getMolecularAfr() const { return m_molecularAfr; }

    protected:
        std::string m_name;
        double m_molecularMass;
        double m_energyDensity;
        double m_density;
        double m_molecularAfr;
        double m_maxBurningEfficiency;
        double m_burningEfficiencyRandomness;
        double m_lowEfficiencyAttenuation;
        double m_maxTurbulenceEffect;
        double m_maxDilutionEffect;

        Function *m_turbulenceToFlameSpeedRatio;
        double m_laminarPeakMixture = 1.21;
        double m_laminarPeakSpeed = units::distance(30.5, units::cm) / units::sec;
        double m_laminarSpeedCurvature = -units::distance(54.9, units::cm) / units::sec;
        double m_laminarAlpha0 = 2.4, m_laminarAlpha1 = -0.271, m_laminarAlpha2 = 3.51;
        double m_laminarBeta0 = -0.357, m_laminarBeta1 = 0.14, m_laminarBeta2 = 2.77;
};

#endif /* ATG_ENGINE_FUEL_H */
