#pragma once
/**
 * @file airbrake.h
 * @brief Airbrake interface
 * @note This module manages the state machine for the airbrake with a focus
 *       on deterministic timing and sensor fusion to counterract air resistance. 
 * @author Joseph Chang
*/

#include "bno055_imu.h"
// #include "imu_math.h"
// #include "kalman.h"
#include "sensor_data.h"
#include "servo_motor"

namespace LBR
{

enum class AirbrakeState : uint8_t
{
    PRELAUNCH = 0,             // Not launched yet
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
};

class AirBrake
{
public:
    AirBrake(AirbrakeParams);

    /**
     * @brief Returns the current state of the airbrake
     * @param None
     * @return AirBrakeState
     */
    AirbrakeState get_state(void) const;

    /**
     * @brief Updates the state machine of the airbrake based on current state, velocity,
     * and altitude. Drives the servo motor to deploy and retract airbrakes. 
     * 
     * Velocity and altitude will be derived from the pressure, temperature, and imu data.
     * 
     * @param void
     * @return None
     */
    void update(SensorData data);

private:
    AirbrakeState state = AirbrakeState::PRELAUNCH;

    float init_pressure, pressure, base_altitude;
    ServoMotor motor;
    LBR::Bno055Data imu;
};

}  // namespace LBR