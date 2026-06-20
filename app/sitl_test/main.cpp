#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>
#include "apps/airbrake_app.cc"

using json = nlohmann::json;
using namespace std;

// How to run (works for myself, may be different for others):
// 1. cmd + shift + p
// 2. Tasks: Run Task
// 3. Build SITL
// 4 ./app/sitl_test/main

// Simple SITL simulation for testing AirbrakeApp logic
class SITL : public AirbrakeApp { 
    private:
        double time_s{0.0};
        double time_step_s{0.1};
        double max_simulation_time{120.0};
        double altitude_m{0.0};
        double velocity_mps{0.0};
        double acceleration_mps2{-9.81};

        // Load simulation parameters and initial conditions from JSON config file
        void loadSimulationConfig(const string& config_path) {
            ifstream file(config_path);

            if (!file.is_open()) {
                cerr << "Failed to open simulation config: " << config_path << endl;
                return;
            }

            json j;
            file >> j;

            time_step_s = j["simulation_parameters"].value("time_step", 0.1);
            max_simulation_time = j["simulation_parameters"].value("max_simulation_time", 120.0);
            altitude_m = j["variables"].value("altitude_m", 0.0);
            velocity_mps = j["variables"].value("velocity_mps", 0.0);
            acceleration_mps2 = j["variables"].value("acceleration_mps2", -9.81);

        }

    public:
        // Initialize AirbrakeApp with config file and simulation parameters
        SITL() : AirbrakeApp("app/sitl_test/config/airbrake.json") {
            loadSimulationConfig("app/sitl_test/config/airbrake.json");
        }

        void run() {
            AirbrakeApp::init();
            // Run until rocket reaches ground or timeout occurs
            while (altitude_m >= 0.0 && time_s <= max_simulation_time) {
                SensorData data{
                    time_s, altitude_m, velocity_mps, acceleration_mps2
                };
                AirbrakeApp::update(data);

                AirbrakeState airbrake = AirbrakeApp::getCommand();
                RecoveryState recovery = AirbrakeApp::getRecoveryCommand();

                double airbrake_drag_accel = 0.0;
                if (airbrake.deployment) {
                    if (velocity_mps > 0.0) {
                        airbrake_drag_accel = -5.0; // Simplified drag effect when moving upwards
                    }
                    else if (velocity_mps < 0.0) {
                        airbrake_drag_accel = 20.0; // Simplified drag effect when moving downwards
                    }
                }

                double recovery_drag_accel = 0.0;
                if (recovery.deployment && velocity_mps < 0.0) {
                    recovery_drag_accel = 60.0; // Simplified drag effect of recovery system when descending
                }
                acceleration_mps2 = -9.81 + airbrake_drag_accel + recovery_drag_accel;
                velocity_mps += acceleration_mps2 * time_step_s;
                altitude_m += velocity_mps * time_step_s;
                time_s += time_step_s;
            }
            cout << "Simulation ended at time: " << time_s << " seconds." << endl;
        }
};

int main() {
    SITL sitl;
    sitl.run();
    return 0;
}