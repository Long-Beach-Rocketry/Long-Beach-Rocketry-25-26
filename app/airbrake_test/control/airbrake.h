#pragma once
/**
 * @file airbrake.h
 * @brief Airbrake interface
 * @note This module manages the state machine for the airbrake with a focus
 *       on deterministic timing and sensor fusion to counterract air resistance. 
*/

/**                                
* 1. Software loop                                              
* - Read IMU for acceleration and orientation 
*   + accel, gyro, linear_accel, gravity, quat
* - Read Baro for altitude    
*   + ...
*                         
* - State machine:                            
* + WAITING - Locked    
* + ASCENT  - PID Active  
* + APOGEE  - Retract (Safety)      
* 
*                                             Peak Altitude
*                                                   _
*                        Target Apogee ----->      / \      
*                                                 /   \
*                                                /     \
*                                               /       \     
*                                              /         \        
*         Waiting Starts        Ascent --->   /           \
* _______________v___________________________/             \
* 
* 
* ? Not sure if we still need to use Kalman filter to calc
*   current velocity to compare with target velocity
*
* 3. Fault tolerance (Safety)
* - Dual Trigger deploy IF (G-Force > X AND Altitude > Y)
* - Servo Watchdog cuts powers to servo motor if it draws too much current
* NOTE : Trust IMU for direction, Baro for height
* 
* 4. Input :
* - CAN Input for LAUNCH and TARGET APOGEE from Telem board
* - CAN Output send Servo angle and System Health to Telem board
* - PWM Output drives the servo motor
*/

// #include "board.h"
// #include "imu_math.h"
// #include "kalman.h"
// #include "motor"

namespace LBR
{


    enum class AirbrakeState
{
    WAITING,
    ASCENT,
    APOGEE,
    DEATH, // temp val, if something has gone terrible wrong
};

class AirBrake
{
public:
    AirBrake(motor);

    AirbrakeState getState() const;

    void fetchImuData(const LBR::Bno055Data& data); // Fetch IMU Data

    void fetchPressData(const float data);  // Fetch Pressure data (hPA)
    
    void fetchTempData(const float data);  // Fetch Temperature data (Celsius)

    void update();  // State machine update

private:

    float calcAltitude() const;

    AirbrakeState state = AirbrakeState::WAITING;
    float pressure, temperature;
    ServoMotor motor;
    LBR::Bno055Data imu;
    // Motor& ;
};

}  // namespace LBR