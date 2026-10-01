/*
* @file main.cc
* @author Mario Cruz
* @brief SITL test harness for Rocket Dynamics Model + Atmosphere Model
* @date 9-30-2026
*/

#include <algorithm>
#include <cmath>
#include <iostream>

#include "atmosphere.h"
#include "atmosphere_model.h"
#include "rocket_dynamics.h"

// Private namespace
namespace {
    // Variable to convert meters to feet
    constexpr double MetersToFeet = 3.28084;

    // Fake motor standing in for real thrust curve, called per simulated timestep
    // TODO: Replace with the real motor thrust curve
    double motor_thrust_n(double time_s) {
        constexpr double MotorBurnDuration_s = 6.0;
        constexpr double MotorThrust_N = 800.0;
        if (time_s >= 0.0 && time_s < MotorBurnDuration_s) {
            return MotorThrust_N;
        }
        return 0.0;
    }

    // Rocket assumed to be pointing straight up ALL the time, ngl this is a 1D rocket simulation not 2D
    double body_angle_deg(double time_s) {
        (void)time_s;   // Keep this or code breaks
        return 0.0;
    }

    // Rocket states for printing and I guess some logic
    enum class FlightPhase {
        Prelaunch,
        Launched,
        AirbrakesDeploying,
        AirbrakesFullyDeployed,
        AirbrakesRetracting,
        AirbrakesFullyRetracted,
        RecoveryPopped,
    };

    // Translating enum back to readable text for printing
    const char* to_string(FlightPhase phase) {
        switch (phase) {
            case FlightPhase::Prelaunch: return "PRELAUNCH";
            case FlightPhase::Launched: return "LAUNCHED";
            case FlightPhase::AirbrakesDeploying: return "AIRBRAKES_DEPLOYING";
            case FlightPhase::AirbrakesFullyDeployed: return "AIRBRAKES_FULLY_DEPLOYED";
            case FlightPhase::AirbrakesRetracting: return "AIRBRAKES_RETRACTING";
            case FlightPhase::AirbrakesFullyRetracted: return "AIRBRAKES_FULLY_RETRACTED";
            case FlightPhase::RecoveryPopped: return "RECOVERY_POPPED";
        }
        return "UNKNOWN";
    }

    // Threshold to start retraction before true peak
    constexpr double ApogeeVelocityThreshold_mps = 0.5;

    // Placeholder returns flap angle bsed on current deployment condition
    // MPC will replace this in the future
    double target_flap_angle_deg(FlightPhase phase) {
        switch (phase) {
            case FlightPhase::AirbrakesDeploying:
            case FlightPhase::AirbrakesFullyDeployed:
                return AIRBRAKE_DEPLOYED_DEG;
            default:
                return AIRBRAKE_RETRACTED_DEG;
        }
    }

    // If results are funky, -1 if never hit, 0 default
    struct SimulationResult {
        double max_altitude_m = 0.0;
        double motor_burnout_time_s = -1.0;
        double fully_deployed_time_s = -1.0;
        double apogee_time_s = -1.0;
        double fully_retracted_time_s = -1.0;
        double recovery_popped_time_s = -1.0;
        double final_time_s = 0.0;
    };

    // Initializing stuff
    SimulationResult run_simulation(
        bool airbrakes_enabled,
        const GroundConditions& ground) {

        // Rocket's starting states
        // TODO: verify if mass is correct
        RocketVariables current = {};
        current.time_s = 0.0;
        current.x_m = 0.0;
        current.y_m = 0.0;
        current.velocity_x_mps = 0.0;
        current.velocity_y_mps = 0.0;
        current.acceleration_x_mps2 = 0.0;
        current.acceleration_y_mps2 = 0.0;
        current.mass_kg = 20.0;

        // Standin static Rocket properties
        // TODO: find real measurements
        RocketStaticProperties property = {};
        property.rocket_coefficient_of_drag = 0.50;
        property.rocket_reference_area_m2 = 0.01;
        property.flap_coefficient_of_drag = 1.0;
        property.flap_reference_area_m2 = 0.0005;
        property.number_of_flaps = 2;

        RocketDynamicProperties sim = {};
        sim.dt_s = 0.1;
        sim.gravity_mps2 = 9.81;

        const double maximum_simulation_time_s = 1200.0;

        SimulationResult result;
        result.max_altitude_m = current.y_m;

        FlightPhase phase = FlightPhase::Prelaunch;
        bool was_thrusting = false;
        bool has_launched = false;
        int simulation_step = 0;

        // Actual simulation loop, stops after landing or time runout
        while (current.time_s < maximum_simulation_time_s) {
            
            // Dynamic sim properties supplier
            sim.thrust_n = motor_thrust_n(current.time_s);
            sim.body_angle_deg = body_angle_deg(current.time_s);
            sim.air_density_kgm3 = atmosphere_density_kgm3(current.y_m, &ground);

            // Stand in airbrake logic & actuator model
            const FlightPhase current_phase = phase;

            if (current_phase == FlightPhase::Prelaunch && sim.thrust_n > 0.0) {
                phase = FlightPhase::Launched;
            }
            if (airbrakes_enabled &&
                current_phase == FlightPhase::Launched && was_thrusting && sim.thrust_n <= 0.0) {
                result.motor_burnout_time_s = current.time_s;
                phase = FlightPhase::AirbrakesDeploying;
            }
            if (current_phase == FlightPhase::AirbrakesDeploying) {
                result.fully_deployed_time_s = current.time_s;
                phase = FlightPhase::AirbrakesFullyDeployed;
            }
            // Apogee can interrupt either deploy phase and forces a retract.
            if ((current_phase == FlightPhase::AirbrakesDeploying ||
                 current_phase == FlightPhase::AirbrakesFullyDeployed) &&
                current.velocity_y_mps <= ApogeeVelocityThreshold_mps) {
                result.apogee_time_s = current.time_s;
                phase = FlightPhase::AirbrakesRetracting;
            }
            if (current_phase == FlightPhase::AirbrakesRetracting) {
                result.fully_retracted_time_s = current.time_s;
                phase = FlightPhase::AirbrakesFullyRetracted;
            }
            if (current_phase == FlightPhase::AirbrakesFullyRetracted) {
                result.recovery_popped_time_s = current.time_s;
                phase = FlightPhase::RecoveryPopped;
            }
            // If airbrakes turned off, skip straight to recovery at apogee
            if (!airbrakes_enabled && current_phase == FlightPhase::Launched &&
                current.velocity_y_mps <= ApogeeVelocityThreshold_mps) {
                result.apogee_time_s = current.time_s;
                result.recovery_popped_time_s = current.time_s;
                phase = FlightPhase::RecoveryPopped;
            }
            was_thrusting = sim.thrust_n > 0.0;

            property.flap_angle_deg = target_flap_angle_deg(phase);

            // Updating rocket dynamic variables
            current = update_rocket_dynamics(&current, &property, &sim);

            const double altitude_m = current.y_m;
            const double vertical_velocity_mps = current.velocity_y_mps;
            result.max_altitude_m = std::max(result.max_altitude_m, altitude_m);

            // Updating atmosphere variables
            const BarometerRawValues baro_values =
                compute_barometer_raw_values(altitude_m, &ground);
                (void)baro_values;   // need or crash

            if (altitude_m > 0.0 || vertical_velocity_mps > 0.0) {
                has_launched = true;
            }

            // Printing every 0.1 second step
            if (simulation_step % 10 == 0) {
                std::cout
                    << "t=" << current.time_s
                    << " s, altitude=" << altitude_m
                    << " m, vertical velocity=" << vertical_velocity_mps
                    << " m/s, phase=" << to_string(phase) << '\n';
            }

            // Landing detecting and breaks loop
            if (has_launched && current.time_s > result.motor_burnout_time_s &&
                altitude_m <= 0.0 && vertical_velocity_mps < 0.0) {
                current.y_m = 0.0;
                current.velocity_y_mps = 0.0;
                break;
            }

            ++simulation_step;
        }

        result.final_time_s = current.time_s;

        // Print final data onto terminal
        std::cout
            << "Motor burnout (-> AIRBRAKES_DEPLOYING): " << result.motor_burnout_time_s << " s\n"
            << "Fully deployed: " << result.fully_deployed_time_s << " s\n"
            << "Apogee (-> AIRBRAKES_RETRACTING): " << result.apogee_time_s << " s\n"
            << "Fully retracted: " << result.fully_retracted_time_s << " s\n"
            << "Recovery popped: " << result.recovery_popped_time_s << " s\n"
            << "Maximum altitude: " << result.max_altitude_m << " m ("
            << result.max_altitude_m * MetersToFeet << " ft)\n"
            << "Final time: " << result.final_time_s << " s\n";

        return result;
    }

}  // namespace

int main() {
    // Build GroundConsitiions struct, placeholder elevation at 1200m
    const GroundConditions ground = ground_conditions_for_elevation_m(1200.0);
    std::cout << "Ground conditions: " << ground.temperature_k << " K, "
              << ground.pressure_pa << " Pa (elevation 1200 m placeholder -- "
              << "replace with measured launch-day values)\n\n";

    // Currently testing with and without airbrakes to compare, don't comment one out
    const SimulationResult with_airbrakes = run_simulation(true, ground);
    const SimulationResult without_airbrakes = run_simulation(false, ground);

    // Saving results of with and without to compare
    const double apogee_with_ft = with_airbrakes.max_altitude_m * MetersToFeet;
    const double apogee_without_ft = without_airbrakes.max_altitude_m * MetersToFeet;
    const double delta_ft = apogee_with_ft - apogee_without_ft;

    // Print results in terminal
    std::cout << "=== Airbrake effectiveness summary ===\n"
              << "Apogee without airbrakes: " << apogee_without_ft << " ft\n"
              << "Apogee with airbrakes:    " << apogee_with_ft << " ft\n"
              << "Altitude reduction:       " << delta_ft << " ft "
              << "(target: approx. -1000 ft)\n"
              << "Target apogee w/ airbrakes: 7500 ft "
              << "(actual: " << apogee_with_ft << " ft)\n";

    return 0;
}