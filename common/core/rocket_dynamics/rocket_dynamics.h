/*
* @file rocket_dynamics.h
* @author Mario Cruz
* @brief Data structures and function declared for Rocket Dynamics Model
* @date 9-30-2026
*/

#ifndef ROCKET_DYNAMICS_H
#define ROCKET_DYNAMICS_H

// Named reference values for airbrake retraction and deployment degrees
// In SITL pipeline, flap angle arrives already resolves from Actuator Model
#define AIRBRAKE_RETRACTED_DEG 0.0
#define AIRBRAKE_DEPLOYED_DEG 30.0

// Rocket's variables (time, position, mass, etc) at any given instant
typedef struct {
    double time_s;
    double x_m;
    double y_m;
    double velocity_x_mps;
    double velocity_y_mps;
    double acceleration_x_mps2;
    double acceleration_y_mps2;
    double mass_kg;
} RocketVariables;

// Rocket's physical characteristics that remain constant during simulation
// The flap angle is controlled separately and may change during simulation
// Referenced as Static Properties in LBR Airbrake Simulation Testing Pipeline
typedef struct {
    double rocket_coefficient_of_drag;
    double rocket_reference_area_m2;
    double flap_coefficient_of_drag;
    double flap_reference_area_m2;
    int number_of_flaps;
    double flap_angle_deg;              // Flap angle does change
} RocketStaticProperties;

// Rocket properties that may change during simulation.
// Referenced as Dynamic Properties in LBR Airbrake Simulation Testing Pipeline
typedef struct {
    double dt_s;
    double thrust_n;
    double body_angle_deg;
    double gravity_mps2;
    double air_density_kgm3;
} RocketDynamicProperties;

#ifdef __cplusplus
extern "C" {
#endif

// Updates the rocket's dynamic states in one simulation time step
RocketVariables update_rocket_dynamics(
    const RocketVariables* current,
    const RocketStaticProperties* property,
    const RocketDynamicProperties* sim
);

#ifdef __cplusplus
}
#endif
#endif