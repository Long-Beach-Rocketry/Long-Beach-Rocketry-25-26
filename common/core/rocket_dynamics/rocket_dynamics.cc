/* 
* @file rocket_dynamics.cc
* @author Mario Cruz
* @brief Implements the physics calculations for Rocket Dynamic Model
* @date 9-30-2026
*/

// Implementing the data declared in ""rocket_dynamics.h"
#include "rocket_dynamics.h"
#include <math.h>

#define ROCKET_PI 3.14159265358979323846
#define DEG_TO_RAD (ROCKET_PI / 180.0)

RocketVariables update_rocket_dynamics(
    const RocketVariables* current,
    const RocketStaticProperties* property,
    const RocketDynamicProperties* sim
)
{
    // "Next" is what gets modified below and returned
    // "Current" (a const pointer) is never touched
    RocketVariables next = *current;

    // Convert flap angle from degrees to radians for calculations
    const double flap_angle_rad = property->flap_angle_deg * DEG_TO_RAD;
    
    // Calculate the effective surface area of the flaps based on their angle
    const double effective_single_flap_area_m2 = property->flap_reference_area_m2 * sin(flap_angle_rad);

    // Calculate the total drag area considering both the rocket body and the flaps
    const double total_cd_area_m2 = 
        (property->rocket_coefficient_of_drag * property->rocket_reference_area_m2) +
        (property->number_of_flaps * property->flap_coefficient_of_drag * effective_single_flap_area_m2);

    // Calculate the overall speed of the rocket using its velocity components
    const double speed_mps = hypot(current->velocity_x_mps, current->velocity_y_mps);

    // Drag equation (F = 1/2 * rho * v^2 * Cd * A)
    // Drag automatically points opposite of whatever diretcion the rocket's actually moving
    const double drag_force_n = 0.5 * sim->air_density_kgm3 * speed_mps * total_cd_area_m2;
    const double drag_x_n = -drag_force_n * current->velocity_x_mps;
    const double drag_y_n = -drag_force_n * current->velocity_y_mps;

    // Calculate the thrust components based on the body angle
    // Splits thrust into x/y based on which way the rocket's tilted
    const double body_angle_rad = sim->body_angle_deg * DEG_TO_RAD;
    const double thrust_x_n = sim->thrust_n * sin(body_angle_rad);
    const double thrust_y_n = sim->thrust_n * cos(body_angle_rad);

    // Update rocket's acceleration based on a=F/m, y axis affected by gravity
    next.acceleration_x_mps2 = (thrust_x_n + drag_x_n) / current->mass_kg;
    next.acceleration_y_mps2 = (thrust_y_n + drag_y_n) / current->mass_kg - sim->gravity_mps2;

    // Update rocket's position based on x/y = x/y + v * t + 1/2 * a * t^2
    next.x_m = current->x_m + current->velocity_x_mps * sim->dt_s + 0.5 * next.acceleration_x_mps2 * sim->dt_s * sim->dt_s;
    next.y_m = current->y_m + current->velocity_y_mps * sim->dt_s + 0.5 * next.acceleration_y_mps2 * sim->dt_s * sim->dt_s;

    // Update rocket's velocity based on v = v + a * t
    next.velocity_x_mps = current->velocity_x_mps + next.acceleration_x_mps2 * sim->dt_s;
    next.velocity_y_mps = current->velocity_y_mps + next.acceleration_y_mps2 * sim->dt_s;

    // Update the simulation time for the next step
    next.time_s = current->time_s + sim->dt_s;
    
    return next;
}