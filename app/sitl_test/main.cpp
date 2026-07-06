#include <iostream>
#include <fstream>
#include <cmath>

#include <nlohmann/json.hpp>
#include "apps/airbrake_app.cc"

using json = nlohmann::json;
using namespace std;

// How to run (works for myself, may be different for others):
// 1. cmd + shift + p
// 2. Tasks: Run Task
// 3. Build SITL
// 4 ./app/sitl_test/main

// SITL simulation for testing AirbrakeApp logic
class SITL : public AirbrakeApp { 
    private:
        // Declaring simulation variables with default values
        double time_s{0.0};
        double time_step_s{0.1};
        double max_simulation_time{600.0};

        double altitude_m{0.0};
        double velocity_mps{150.0};
        double acceleration_mps2{0.0};

        double mass_kg{20.0};
        double air_density_kgpm3{1.225};

        double cd_flap{1.2};
        double cd_rocket{0.75};
        double flap_area_m2{0.004};
        double rocket_area_m2{0.008};

        double flap_angle_deg{30.0};
        double thrust_y_n{500.0};
        double motor_burn_time_s{5.0};
        double gravity_mps2{9.81};


        // Load simulation variables from JSON config file
        void loadSimulationConfig(const string& config_path) {
            ifstream file(config_path);

            if (!file.is_open()) {
                cerr << "Failed to open simulation config: " << config_path << endl;
                return;
            }
            json j;
            file >> j;

            // Assigning values from JSON, with default values if not present
            time_step_s = j["simulation_parameters"].value("time_step", 0.1);
            max_simulation_time = j["simulation_parameters"].value("max_simulation_time", 600.0);

            altitude_m = j["variables"].value("altitude_m", 0.0);
            velocity_mps = j["variables"].value("velocity_mps", 150.0);
            acceleration_mps2 = 0.0;

            mass_kg = j["physics"].value("mass_kg", 20.0);
            air_density_kgpm3 = j["physics"].value("air_density_kgpm3", 1.225);

            cd_flap = j["physics"].value("cd_flap", 1.2);
            cd_rocket = j["physics"].value("cd_rocket", 0.75);
            flap_area_m2 = j["physics"].value("flap_area_m2", 0.004);
            rocket_area_m2 = j["physics"].value("rocket_area_m2", 0.008);

            flap_angle_deg = j["physics"].value("flap_angle_deg", 30.0);
            thrust_y_n = j["physics"].value("thrust_y_n", 500.0);
            motor_burn_time_s = j["physics"].value("motor_burn_time_s", 5.0);
            gravity_mps2 = j["physics"].value("gravity_mps2", 9.81);
        }
        
        // Helper function to compute the effective flap surface area based on the flap angle
        double flapSurfaceArea(double flap_angle_deg) const {
            double flap_angle_rad = flap_angle_deg * M_PI / 180.0;
            return flap_area_m2 * sin(flap_angle_rad);
        }

        // Helper function to compute the vertical drag acceleration based on the flap angle
        double computeVerticalDragAccel(double flap_angle_deg) const {
            double surface_flap_area = flapSurfaceArea(flap_angle_deg);
            double total_drag_area = 2.0 * cd_flap * surface_flap_area + cd_rocket * rocket_area_m2;
            return -(0.5 / mass_kg) * air_density_kgpm3 * total_drag_area * velocity_mps * abs(velocity_mps);
        }

    public:
        // Initialize AirbrakeApp with config file and simulation variables
        SITL() : AirbrakeApp("app/sitl_test/config/airbrake.json") {
            loadSimulationConfig("app/sitl_test/config/airbrake.json");
        }

        void run() {
            AirbrakeApp::init();
            // Run until rocket reaches ground or time exceeds max_simulation_time
            while (time_s <= max_simulation_time) {
                SensorData data{
                    time_s, altitude_m, velocity_mps, acceleration_mps2
                };
                AirbrakeApp::update(data);

                AirbrakeState airbrake = AirbrakeApp::getCommand();
                RecoveryState recovery = AirbrakeApp::getRecoveryCommand();

                double flap_angle_degree = airbrake.deployment ? flap_angle_deg : 0.0;
                double drag_accel = computeVerticalDragAccel(flap_angle_degree);
                double recovery_drag_accel = 0.0;
                
                if (recovery.deployment && velocity_mps < 0.0) {
                    recovery_drag_accel = 8.0;
                }

                // Compute thrust acceleration only during motor burn time
                double thrust_accel = 0.0;
                if (time_s <= motor_burn_time_s) {
                    thrust_accel = thrust_y_n / mass_kg;
                }
                acceleration_mps2 = drag_accel + thrust_accel - gravity_mps2 + recovery_drag_accel;
                velocity_mps += acceleration_mps2 * time_step_s;
                altitude_m += velocity_mps * time_step_s;
                time_s += time_step_s;

                if (altitude_m <= 0.0 && time_s > 0.0) {
                    altitude_m = 0.0;
                    velocity_mps = 0.0;
                    acceleration_mps2 = 0.0;

                    SensorData final_data{
                        time_s, altitude_m, velocity_mps, acceleration_mps2
                    };
                    AirbrakeApp::update(final_data);
                    break; // Rocket has landed
                }
            }
            cout << "Simulation ended at time: " << time_s << " seconds." << endl;
        }
};

int main() {
    SITL sitl;
    sitl.run();
    return 0;
}