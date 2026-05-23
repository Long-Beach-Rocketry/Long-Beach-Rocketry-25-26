#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>
#include "apps/airbrake_app.cpp"

using json = nlohmann::json;
using namespace std;


class SITL : public AirbrakeApp {
    private:
        double time_s = 0.0;
        double time_step_s = 0.1;
        double max_simulation_time = 120.0;
        double altitude_m = 0.0;
        double velocity_mps = 0.0;
        double acceleration_mps2 = -9.81;

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
        SITL() : AirbrakeApp("app/sitl_test/config/airbrake.json") {
            loadSimulationConfig("app/sitl_test/config/airbrake.json");
        }

        void run() {
            AirbrakeApp::init();
            while (altitude_m >= 0.0 && time_s <= max_simulation_time) {
                SensorData data{
                    time_s, altitude_m, velocity_mps, acceleration_mps2
                };
                AirbrakeApp::update(data);

                AirbrakeState airbrake = AirbrakeApp::getCommand();

                double brake_drag_accel = -airbrake.deployment * 20.0;
                acceleration_mps2 = -9.81 + brake_drag_accel;
                velocity_mps += acceleration_mps2 * time_step_s;
                altitude_m += velocity_mps * time_step_s;
                time_s += time_step_s;
            }
            cout << "Simulation ended at time: " << time_s << " seconds." << endl;
        }
};

int main() {
    SITL sitl;
    sitl.init();
    sitl.run();
    return 0;
}