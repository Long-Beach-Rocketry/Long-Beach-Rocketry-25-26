#pragma "once"
#include "app.h"
#include "../sim/airbrake_command.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;


class AirbrakeApp : public App {
    public:
        // Constructor that loads JSON config file
        explicit AirbrakeApp(const string& config_path) {
            ifstream file(config_path);

            if (!file.is_open()) {
                cerr << "Failed to open config [airbrake]: " << config_path << endl;
                return;
            }
            json j;
            file >> j;

            deployment = j["variables"].value("airbrake_deployment", 0.0);
            target_apogee_m = j["test_variables"].value("target_apogee_m", 3000.0);
            deploy_altitude_m = j["test_variables"].value("deploy_altitude_m", 800.0);
            kp = j["test_variables"].value("kp", 0.001);
            state.deployment = deployment;
        }

        // Called once at the start of simulation
        void init() override {
            cout << "Airbrake App initialized\n";
        }

        // Called every simulation loop
        void update(const SensorData& data) override {
            state.deployment = calculateExtension(data);

            cout
            << "Time: " << data.time_s
            << " | Altitude: " << data.altitude_m
            << " | Velocity: " << data.velocity_mps
            << " | Acceleration: " << data.acceleration_mps2
            << " | Airbrake Deployment: " << state.deployment
            << endl;
        }

        // Return current airbrake command
        AirbrakeState getCommand() const {
            return state;
        }
    
        private:
            double deployment = 0.0;
            double target_apogee_m = 3000.0;
            double deploy_altitude_m = 800.0;
            double kp = 0.001;

            AirbrakeState state;

            // Calculating airbrake extension based on current sensor data and target apogee
            double calculateExtension(const SensorData& data) {
                // Do not deploy below activation altitude
                if (data.altitude_m <= deploy_altitude_m) {
                    return 0.0;
                }
                // Do not deploy if velocity is zero or negative (descending)
                if (data.velocity_mps <= 0.0) {
                    return 0.0;
                }

                constexpr double g = 9.81; // Gravity constant

                double predicted_apogee = data.altitude_m + (data.velocity_mps * data.velocity_mps) / (2.0 * g);

                double error = predicted_apogee - target_apogee_m;
                double output = kp * error;

                if (output < 0.0) output = 0.0;
                if (output > 1.0) output = 1.0;
                return output;
            }
};