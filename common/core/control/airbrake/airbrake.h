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

    bool init();

    /**
     * @brief Returns the current state of the airbrake
     * @param void
     * @return AirBrakeState
     */
    AirbrakeState get_state(void) const;

    /**
     * @brief Simple setter that extracts needed info from the imu
     * @param Bno055Data 
     * @return None
     */
    void fetch_imu(const LBR::Bno055Data& data);

    /**
     * @brief Simple setter extracts info from the barometer.
     * Pressure data (hPA) and temperature data (Celsius).
     * @param float
     * @return None
     */
    void fetch_baro(const Bmp390& bmp390);

    /**
     * @brief Updates the state machine of the airbrake based on current state, velocity,
     * and altitude. Drives the servo motor to deploy and retract airbrakes. 
     * 
     * Velocity and altitude will be derived from the pressure, temperature, and imu data.
     * 
     * @param void
     * @return None
     */
    void update(void);

private:
    AirbrakeState state = AirbrakeState::PRELAUNCH;

    // It appears that temperature is not used (yet).
    float init_pressure, pressure, init_temperature, temperature, base_altitude;
    ServoMotor motor;
    LBR::Bno055Data imu;
};

}  // namespace LBR