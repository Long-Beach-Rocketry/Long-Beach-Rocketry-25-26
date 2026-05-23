#pragma once


// Sensor data which'll be used in every application (airbrake, camera, telemtry)
struct SensorData {
    double time_s;
    double altitude_m;
    double velocity_mps;
    double acceleration_mps2;
};


// Base class other apps will inherit from [an init() and update() function]
class App {
    public:
        virtual void init() = 0;
        virtual void update(const SensorData& data) = 0;
};