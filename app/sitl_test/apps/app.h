#pragma once

// Sensor data which'll be used in every application (airbrake, camera, telemtry)
struct SensorData {
    double time_s;
    double altitude_m;
    double velocity_mps;
    double acceleration_mps2;
};

// Abstract base class other apps will inherit from
class App {
    public:
        virtual void init() = 0; // Called once at the start of simulation
        virtual void update(const SensorData& data) = 0; // Called every simulation loop with the latest sensor data
        virtual ~App() = default; // Virtual destructor for proper cleanup
};