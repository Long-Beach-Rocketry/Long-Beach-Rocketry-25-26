/* 
* @file rocket_dynamics.cc
* @author Mario Cruz
* @brief Implements the physics calculations for Rocket Dynamic Model
* @date 9-30-2026
*/

#include "rocket_dynamics.h"
#include <algorithm>
#include <cmath>
#include "flight_constants.h"

namespace LBR
{

using namespace FlightConstants;

namespace
{

constexpr double DegToRad{3.14159265358979323846 / 180.0};
constexpr double GravityMps2{9.80665};

// Total drag coefficient * area of the body and flaps
// CdA = Cd_body * A_body + n * Cd_flap * A_flap * sin(flap angle)
double drag_area_m2(const RocketProperties& properties)
{
    const AirbrakeProperties& flaps = properties.flaps;
    const double flap_drag_area_m2 = flaps.count * flaps.coefficient_of_drag *
                                     flaps.reference_area_m2 *
                                     std::sin(flaps.angle_deg * DegToRad);
    return properties.rocket_coefficient_of_drag *
               properties.rocket_reference_area_m2 +
           flap_drag_area_m2;
}

// Safety check every value a step is real number
bool step_inputs_valid(const RocketVariables& current,
                       const FlightInputs& inputs)
{
    const bool all_finite =
        std::isfinite(current.time_s) && std::isfinite(current.x_m) &&
        std::isfinite(current.y_m) && std::isfinite(current.velocity_x_mps) &&
        std::isfinite(current.velocity_y_mps) &&
        std::isfinite(current.mass_kg) && std::isfinite(inputs.dt_s) &&
        std::isfinite(inputs.thrust_n) &&
        std::isfinite(inputs.body_angle_deg) &&
        std::isfinite(inputs.air_density_kgm3);
    return all_finite && inputs.dt_s > 0.0 && current.mass_kg > 0.0 &&
           inputs.air_density_kgm3 >= 0.0;
}

}  // namespace

RocketDynamicModel::RocketDynamicModel(const RocketProperties& properties_)
    : properties(properties_)
{
    if (!set_flap_angle_deg(properties.flaps.angle_deg))
    {
        properties.flaps.angle_deg = AirbrakeRetractedDeg;
    }
}

// --------------------------------------------------------------------------------
//   Drag:          F_drag = 1/2 * rho * v^2 * CdA, opposite the motion
//   Thrust:        F_thrust_x = T * sin(theta),  F_thrust_y = T * cos(theta)
//   Acceleration:  a_x = (F_thrust_x + F_drag_x) / m
//                  a_y = (F_thrust_y + F_drag_y) / m - g
//   Position:      x_next = x + v_x * dt + 1/2 * a_x * dt^2   (same for y)
//   Velocity:      v_next = v + a * dt
// --------------------------------------------------------------------------------
bool RocketDynamicModel::step(const RocketVariables& current,
                              const FlightInputs& inputs,
                              RocketVariables& next_out) const
{
    if (!step_inputs_valid(current, inputs))
    {
        return false;
    }

    RocketVariables next = current;
    const double dt = inputs.dt_s;

    // Drag equation, F = 1/2 * rho * v^2 * Cd * A
    const double speed_mps =
        std::hypot(current.velocity_x_mps, current.velocity_y_mps);
    const double drag_per_velocity =
        0.5 * inputs.air_density_kgm3 * speed_mps * drag_area_m2(properties);
    const double drag_x_n = -drag_per_velocity * current.velocity_x_mps;
    const double drag_y_n = -drag_per_velocity * current.velocity_y_mps;

    // Thrust split into x and y by the body tilt
    const double body_angle_rad = inputs.body_angle_deg * DegToRad;
    const double thrust_x_n = inputs.thrust_n * std::sin(body_angle_rad);
    const double thrust_y_n = inputs.thrust_n * std::cos(body_angle_rad);

    // a = F / m
    next.acceleration_x_mps2 = (thrust_x_n + drag_x_n) / current.mass_kg;
    next.acceleration_y_mps2 =
        (thrust_y_n + drag_y_n) / current.mass_kg - GravityMps2;

    // Position, x = x + v * t + 1/2 * a * t^2
    next.x_m = current.x_m + current.velocity_x_mps * dt +
               0.5 * next.acceleration_x_mps2 * dt * dt;
    next.y_m = current.y_m + current.velocity_y_mps * dt +
               0.5 * next.acceleration_y_mps2 * dt * dt;

    // Velocity, v = v + a * t
    next.velocity_x_mps =
        current.velocity_x_mps + next.acceleration_x_mps2 * dt;
    next.velocity_y_mps =
        current.velocity_y_mps + next.acceleration_y_mps2 * dt;

    // Time, t = t + dt
    next.time_s = current.time_s + dt;
    // Safety check
    if (!std::isfinite(next.x_m) || !std::isfinite(next.y_m) ||
        !std::isfinite(next.velocity_x_mps) ||
        !std::isfinite(next.velocity_y_mps))
    {
        return false;
    }
    next_out = next;
    return true;
}

// Clamp flap angle within the airbrake's range
bool RocketDynamicModel::set_flap_angle_deg(double angle_deg)
{
    if (!std::isfinite(angle_deg))
    {
        return false;
    }
    properties.flaps.angle_deg =
        std::clamp(angle_deg, AirbrakeRetractedDeg, AirbrakeDeployedDeg);
    return true;
}

double RocketDynamicModel::get_flap_angle_deg() const
{
    return properties.flaps.angle_deg;
}

}  // namespace LBR