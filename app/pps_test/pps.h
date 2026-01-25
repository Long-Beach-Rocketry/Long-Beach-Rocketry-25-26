#pragma once
/**
 * @file pps.h
 * @brief PPS module interface
 * @note This module manages the PPS state machine for deployment, rotation, and retraction.
 * @author Bex Saw
 * @date 2026/01/05
 */
#include "motor_support/dc_motor.h"
#include "imu_math.h"
#include "board.h"
#include "pps_helpers.h" 

namespace LBR {

enum class PpsState {
    Idle,         // Waiting for command, not moving
    Deploying,    // Deploy to limit switch
    Rotating,     // Move to target/drill position
    Retract       // Retract mechanism
};

class Pps {
    LBR::Quaternion state_quat_{};
    LBR::Vec3 state_accel_{};
public:
    PpsState getState() const;
    Pps(Gpio& gpio, Motor& motor);
    void fetchImuData(const LBR::Quaternion& data); // Fetch IMU data for quaternion
    void fetchAccelData(const LBR::Vec3& data); // Fetch IMU data for acceleration
    void update(); // State machine update, no IMU arg

private:
    Gpio& gpio_;
    Motor& motor_; 
    
    PpsState state_ = PpsState::Idle; // Initial state is Idle

    /**
     * @brief Read the current state of the limit switch.
     * @return LimitSwitchState (Retracted or Extended)
     * @note Limit switch state: 0 = Retracted, 1 = Extended
     */
    enum class LimitSwitchState { retracted = 0, extended = 1 };
    LimitSwitchState readLimitSwitch();

    /**
     * @brief Check if deploy sequence should start.
     * @return true if deployment should begin, false otherwise.
     * @note Used to determine when to transition from Idle to Deploy state.
     */
    bool deploy();

    /**
     * @brief Check if mechanism is retracted. 
     * @return true if retracted, false otherwise.
     * @note Used to determine when to transition from Retract to Done state.
     */
    bool retracted();

   /**
    * @brief Check if rotation to target/drill position is complete.
    * @return true if rotation is complete, false otherwise.
    * @note Should use encoder, IMU, or other feedback in real implementation. (brainstorming)
    */
    bool rotationComplete();
};

} // namespace LBR
