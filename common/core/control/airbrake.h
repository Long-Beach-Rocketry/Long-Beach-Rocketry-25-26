#pragma once
/**
 * @file airbrake.h
 * @brief Airbrake interface
 * @note This module manages the state machine for the airbrake with a focus
 *       on deterministic timing and sensor fusion to counterract air resistance. 
*/

#include "bno055_imu.h"
// #include "imu_math.h"
// #include "kalman.h"
// #include "motor"

namespace LBR
{

enum class AirbrakeState
{
    PRELAUNCH,                 // Not launched yet
    LAUNCHED,                  // 6 second interval from launch
    AIRBRAKES_DEPLOYING,       // Deploy airbrakes after 6 seconds
    AIRBRAKES_FULLY_DEPLOYED,  // When at 30 degrees; not sure when to know though
    AIRBRAKES_RETRACTING,      // Should do before target apogee
    AIRBRAKES_FULLY_RETRACTED,  // Again, dont know how to know if retracted
    RECOVERY_POPPED,            // Job finished
};

struct AirbrakeParams
{
    ServoMotor motor;
    // Not sure what to put in here besides servo motor tbh.
    // Maybe imu and baro, maybe not
};

class AirBrake
{
public:
    AirBrake(AirbrakeParams);

    bool init();

    AirbrakeState getState() const;

    void fetchImuData(const LBR::Bno055Data& data);  // Fetch IMU Data

    void fetchPressData(const float data);  // Fetch Pressure data (hPA)

    void fetchTempData(const float data);  // Fetch Temperature data (Celsius)

    void update();  // State machine update

private:
    float calcAltitude() const;

    AirbrakeState state = AirbrakeState::PRELAUNCH;
    float pressure, temperature, base_altitude;
    ServoMotor motor;
    LBR::Bno055Data imu;
    // Motor& ;
};

}  // namespace LBR