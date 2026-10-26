/*
* @file mpc.h
* @author Mario Cruz
* @brief Model Predictive Controller for the airbrake flaps
* @date 10-25-2026
*/

#pragma once
#include "atmosphere.h"
#include "rocket_dynamics.h"

namespace LBR
{

/**
 * @brief Parameters fixed for the whole flight
 */
struct MPCParams
{
    RocketProperties rocket;    // Body and flap characteristics
    double burnout_mass_kg;     // Rocket mass after fuel is burned
    double target_apogee_m;     // Target apogee above launch pad
    double predict_dt_s{0.05};  // Seconds of flight per simulated step
    int max_predict_steps{
        2000};                  // Step limit per prediction, 2000 * 0.5 = 100s
    int search_iterations{10};  // 10 halvings, 30 / 2^10 = 0.03 deg accuracy
};

class MPC
{
public:
    /**
     * @brief Store the MPC parameters and atmosphere used every prediction
     * @param params_ Rocket, target apogee, and prediction settings
     * @param atmosphere_ Atmosphere built from launch pad conditions
     */
    MPC(const MPCParams& params_, const Atmosphere& atmosphere_);

    /**
     * @brief Predicts apogee at current held flap angle
     * @param apogee_out Predicted apogee currently reaching
     * @return true if successful, false otherwise
     */
    bool predict_apogee_m(double altitude_m, double velocity_mps,
                          double flap_angle_deg, double& apogee_out) const;

    /**
     * @brief Finds the best flap angle for target apogee
     * @param angle_out Flap angle (deg)
     * @return true if successful, false otherwise
     */
    bool best_flap_angle_deg(double altitude_m, double velocity_mps,
                             double& angle_out) const;

private:
    MPCParams params;
    Atmosphere atmosphere;
};

}  // namespace LBR