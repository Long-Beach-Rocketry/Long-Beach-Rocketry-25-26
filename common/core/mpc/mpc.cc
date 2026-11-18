#include "mpc.h"
#include <cmath>

namespace LBR
{
// absort kalman data

MPC::MPC(const MPCParams& params_, const Atmosphere& atmosphere_) : params{params_}, atmosphere{atmosphere_}
{}

// mathy parts, fill in later
bool MPC::predict_apogee_m(double altitude_m, double velocity_mps, double flap_angle_deg, double& apogee_out) const
{
    if (!std::isfinite(altitude_m) || !std::isfinite(velocity_mps) || !std::isfinite(flap_angle_deg))
    {
        return false;
    }

    RocketDynamicModel rocket{params.rocket};
    if (!rocket.set_flap_angle_deg(flap_angle_deg))
    {
        return false;
    }

    RocketVariables state{};
    state.y_m = altitude_m;
    state.velocity_y_mps = velocity_mps;
    state.mass_kg = params.burnout_mass_kg;

    FlightInputs inputs{params.predict_dt_s, 0.0, 0.0, 0.0};

    for (int i = 0; i < params.max_predict_steps && state.velocity_y_mps > 0.0; ++i)
    {
        if (!atmosphere.density_kgm3(state.y_m, inputs.air_density_kgm3))
        {
            return false;
        }
        if (!rocket.step(state, inputs, state))
        {
            return false;
        }
        
        apogee_out = state.y_m;
        return true;
    }

}

bool MPC::best_flap_angle_deg(double altitude_m, double velocity_mps,
                              double& angle_out) const
{
    double low = AirbrakeRetractedDeg;
    double high = AirbrakeDeployedDeg;
    const double target = params.target_apogee_m;
    double apogee = 0.0;
 
    // Undershooting
    if (!predict_apogee_m(altitude_m, velocity_mps, low, apogee))
    {
        return false;
    }
    if (apogee <= target)
    {
        angle_out = low;
        return true;
    }
 
    // overshooting
    if (!predict_apogee_m(altitude_m, velocity_mps, high, apogee))
    {
        return false;
    }
    if (apogee >= target)
    {
        angle_out = high;
        return true;
    }
 
    // bisectional search
    for (int i = 0; i < params.search_iterations; ++i)
    {
        const double mid = 0.5 * (low + high);
        if (!predict_apogee_m(altitude_m, velocity_mps, mid, apogee))
        {
            return false;
        }
        if (apogee > target)
        {
            low = mid;
        }
        else
        {
            high = mid;
        }
    }
 
    angle_out = 0.5 * (low + high);
    return true;
}

}  // namespace LBR