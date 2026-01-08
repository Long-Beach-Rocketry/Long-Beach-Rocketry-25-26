/**
 * @file pps.h
 * @brief PPS module interface
 * @note This module manages the positioning system state machine.
 * @author Bex Saw
 * @date 2025/12/31
 */
 
#pragma once
#include "bsp/motor_if.h"
#include <cstdint>
// #include "bno055_imu.h"
// #include "st_pwm.h" << If STM32L4
// #include "pwm.h" << If generic

namespace LBR {

/**
 * @brief PPS state machine states.
 * @note Idle - waiting for command.
 *       Homing - moving to home position.
 *       MovingToTarget - moving to target position.
 *       AtEndstop - reached endstop.
 *       Fault - error state.
 */
enum class PpsState {
    Idle,
    Homing,
    MovingToTarget,
    AtEndstop,
    Fault
};

/**
 * @brief PPS context structure.
 * @note Holds the state and sensor data for the PPS module.
 */
class Pps {
public:
    Pps();
    void update();
    PpsState getState() const;

private:
    /**
    * @brief Check if homing should start.
    * @note Placeholder for homing condition logic.
    */
    inline bool shouldHome() const {
        // TODO: Add logic to determine if homing should start (e.g., based on command, IMU, etc.)
        return false;
    }
    /**
    * @brief Check for fault conditions.
    * @note Placeholder for fault detection logic.
    */
    inline bool faultCondition() const {
        // TODO: Add logic to detect faults (e.g., IMU error, timeout, etc.)
        return false;
    }
   
    PpsState state_ = PpsState::Idle;
        // IMU data fields (quaternion, etc.)
        //...
        // PWM control fields
        //...

        // GPIO/limit switch state
        bool limit_switch_min_ = false;
        bool limit_switch_max_ = false;
    };

} // namespace LBR
