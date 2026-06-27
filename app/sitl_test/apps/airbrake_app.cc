#pragma once

#include "../sim/airbrake_command.h"
#include "../sim/recovery_command.h"
#include "../sim/rocket_state.h"
#include "app.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
using namespace std;

class AirbrakeApp : public App
{
private:
    AirbrakeState airbrake_command;
    RecoveryState recovery_command;
    RocketState rocket_state{RocketState::PRELAUNCH};

    bool airbrakes_enabled{true};
    bool launch_detected{false};

    double launch_time_s{0.0};
    double deploy_time_s{6.0};
    double retract_time_s{20.0};

    // Threshold values for detecting rocket launch
    const double launch_altitude_threshold_m{10.0};
    const double launch_velocity_threshold_mps{5.0};
    const double launch_acceleration_threshold_mps2{15.0};

public:
    // Constructor that loads JSON config file
    explicit AirbrakeApp(const string& config_path)
    {
        ifstream file(config_path);

        if (!file.is_open())
        {
            cerr << "Failed to open config [airbrake]: " << config_path << endl;
            return;
        }
        json j;
        file >> j;

        airbrakes_enabled = j["airbrake"].value("enabled", true);
        deploy_time_s = j["airbrake"].value("deploy_time_s", 6.0);
        retract_time_s = j["airbrake"].value("retract_time_s", 20.0);
    }

    // Called once at the start of simulation
    void init() override
    {
        rocket_state = RocketState::PRELAUNCH;
        airbrake_command.deployment = false;
        recovery_command.deployment = false;
        launch_detected = false;
        launch_time_s = 0.0;

        cout << "Airbrake App initialized\n";

        cout << fixed << setprecision(2);
        cout << setw(8) << "Time" << setw(12) << "Altitude" << setw(12)
             << "Velocity" << setw(15) << "Acceleration" << setw(12)
             << "Airbrake" << setw(12) << "Recovery" << setw(18)
             << "Rocket State" << endl;
        cout << string(89, '-') << endl;
    }

    // Called every simulation loop
    void update(const SensorData& data) override
    {
        switch (rocket_state)
        {
            case RocketState::PRELAUNCH:
                airbrake_command.deployment = false;
                recovery_command.deployment = false;
                if (detectLaunch(data))
                {
                    launch_detected = true;
                    launch_time_s = data.time_s;
                    rocket_state = RocketState::LAUNCHED;
                }
                break;
            case RocketState::LAUNCHED:
            {
                airbrake_command.deployment = false;
                double time_since_launch = data.time_s - launch_time_s;
                if (airbrakes_enabled && time_since_launch >= deploy_time_s)
                {
                    rocket_state = RocketState::BRAKES_DEPLOYING;
                }
                else if (!airbrakes_enabled && data.velocity_mps <= 0.0)
                {
                    recovery_command.deployment = true;
                    rocket_state = RocketState::RECOVERY_POPPED;
                }
                break;
            }
            case RocketState::BRAKES_DEPLOYING:
                airbrake_command.deployment = true;
                rocket_state = RocketState::BRAKES_DEPLOYED;
                break;
            case RocketState::BRAKES_DEPLOYED:
            {
                airbrake_command.deployment = true;
                double time_since_launch = data.time_s - launch_time_s;
                bool reached_apogee = (data.velocity_mps <= 0.0);
                bool reached_retract_time =
                    (time_since_launch >= retract_time_s);
                if (reached_apogee || reached_retract_time)
                {
                    rocket_state = RocketState::BRAKES_RETRACTING;
                }
                break;
            }
            case RocketState::BRAKES_RETRACTING:
                airbrake_command.deployment = false;
                rocket_state = RocketState::BRAKES_RETRACTED;
                break;
            case RocketState::BRAKES_RETRACTED:
            {
                airbrake_command.deployment = false;
                double time_since_launch = data.time_s - launch_time_s;
                bool reached_apogee = (data.velocity_mps <= 0.0);
                bool reached_recovery_backup_time =
                    (time_since_launch >= retract_time_s + 1.0);
                if (reached_apogee || reached_recovery_backup_time)
                {
                    recovery_command.deployment = true;
                    rocket_state = RocketState::RECOVERY_POPPED;
                }
                break;
            }
            case RocketState::RECOVERY_POPPED:
                airbrake_command.deployment = false;
                recovery_command.deployment = true;
                if (data.altitude_m <= 0.0)
                {
                    rocket_state = RocketState::LANDED;
                }
                break;
            case RocketState::LANDED:
                airbrake_command.deployment = false;
                recovery_command.deployment = true;
                break;
        }
        cout << setw(8) << data.time_s << setw(12) << data.altitude_m
             << setw(12) << data.velocity_mps << setw(15)
             << data.acceleration_mps2 << setw(12)
             << airbrake_command.deployment << setw(12)
             << recovery_command.deployment << setw(18)
             << stateToString(rocket_state) << endl;
    }

    // Return current airbrake command
    AirbrakeState getCommand() const
    {
        return airbrake_command;
    }

    // Return current recovery command
    RecoveryState getRecoveryCommand() const
    {
        return recovery_command;
    }

private:
    bool detectLaunch(const SensorData& data) const
    {
        return (data.altitude_m > launch_altitude_threshold_m) ||
               (data.velocity_mps > launch_velocity_threshold_mps) ||
               (data.acceleration_mps2 > launch_acceleration_threshold_mps2);
    }

    string stateToString(RocketState state) const
    {
        switch (state)
        {
            case RocketState::PRELAUNCH:
                return "PRELAUNCH";
            case RocketState::LAUNCHED:
                return "LAUNCHED";
            case RocketState::BRAKES_DEPLOYING:
                return "BRAKES_DEPLOYING";
            case RocketState::BRAKES_DEPLOYED:
                return "BRAKES_DEPLOYED";
            case RocketState::BRAKES_RETRACTING:
                return "BRAKES_RETRACTING";
            case RocketState::BRAKES_RETRACTED:
                return "BRAKES_RETRACTED";
            case RocketState::RECOVERY_POPPED:
                return "RECOVERY_POPPED";
            case RocketState::LANDED:
                return "LANDED";
            default:
                return "UNKNOWN";
        }
    }
};