/*
* @file rocket_dynamics.h
* @author Mario Cruz
* @brief Data structures and function declared for Rocket Dynamics Model
* @date 9-30-2026
*/

#pragma once

namespace LBR
{

/**
 * @brief Rocket's variables (time, position, mass, etc) at any given instant
 */
struct RocketVariables
{
    double time_s;
    double x_m;
    double y_m;
    double velocity_x_mps;
    double velocity_y_mps;
    double acceleration_x_mps2;
    double acceleration_y_mps2;
    double mass_kg;
};

/**
 * @brief Airbrake properties, each flap should be identical
 */
struct AirbrakeProperties
{
    int count;                   // Number of deployable flaps
    double reference_area_m2;    // Reference area of a single flap
    double coefficient_of_drag;  // Drag coefficient of a single flap
    double angle_deg;            // Current flap angle
};

/** 
 * @brief Physical characteristics of the rocket body and its airbrake flaps
 */
struct RocketProperties
{
    double rocket_coefficient_of_drag;  // Drag coefficient of the body
    double rocket_reference_area_m2;    // Reference area of the body
    AirbrakeProperties flaps;           // Airbrake: count, area, cd, angle
};

/**
 * @brief Values from outside the rocket model that one step needs
 */
struct FlightInputs
{
    double dt_s;              // Time step
    double thrust_n;          // Motor thrust, 0 after burnout
    double body_angle_deg;    // Tilt from vertical, 0 = straight up
    double air_density_kgm3;  // From the atmosphere model
};

class RocketDynamicModel
{
public:
    /**
     * @brief Creates the rocket model
     * @param properties_ Rocket body and airbrake characteristics
     */
    explicit RocketDynamicModel(const RocketProperties& properties_);

    /**
     * @brief Updates the rocket by one time step
     * @param current The rocket's variables right now
     * @param inputs Time step, thrust, tilt, and air density
     * @param next_out Filled with rocket's variables after dt_s seconds
     * @return true if successful, false otherwise
     */
    bool step(const RocketVariables& current, const FlightInputs& inputs,
              RocketVariables& next_out) const;

    /**
     * @brief Sets the flap angle used in drag calculation. MPC will use this to search for best angle.
     * @param angle_deg Requested flap_angle
     * @return true if successful, false otherwise
     */
    bool set_flap_angle_deg(double angle_deg);

    /**
     * @return Current flap angle
     */
    double get_flap_angle_deg() const;

private:
    RocketProperties properties;
};

}  // namespace LBR