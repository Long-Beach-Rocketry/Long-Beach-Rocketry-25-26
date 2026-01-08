/**
* @file pps.cc
* @brief PPS module implementation
* @note This module manages the positioning system state machine.
* @author Bex Saw
* @date 2025/12/31
*/


#include "pps.h"
#include "bsp/motor_if.h"

namespace LBR {

Pps::Pps() {
    state_ = PpsState::Idle;
    // Init IMU, PWM, etc. if needed (not sure yet)
    // limit_switch_min_ = readLmsMin();
    // limit_switch_max_ = readLmsMax();
}

void Pps::update() {
    // Read sensors
    // auto [roll, pitch, yaw] = getOrientation(); from IMU
    // limit_switch_min_ = readLmsMin(); from GPIO
    // limit_switch_max_ = readLmsMax(); from GPIO

    switch (state_) {
        case PpsState::Idle:
            // Idle: Motor stopped, waiting for command
            // PWM: Stop motor
            // Limit switch: Monitor only
            if (shouldHome()) state_ = PpsState::Homing;
            break;
        case PpsState::Homing:
            // Homing: Move motor toward home position
            // PWM: Drive motor in home direction
            // Limit switch: Check home (min) switch, stop when triggered
            if (limit_switch_min_) state_ = PpsState::MovingToTarget;
            break;
        case PpsState::MovingToTarget:
            // MovingToTarget: Move motor toward target/endstop
            // PWM: Drive motor in target direction
            // Limit switch: Check end (max) switch, stop when triggered
            if (limit_switch_max_) state_ = PpsState::AtEndstop;
            break;
        case PpsState::AtEndstop:
            // AtEndstop: Motor stopped at end of travel
            // PWM: Stop motor
            // Limit switch: Wait for reset or new command
            if (faultCondition()) state_ = PpsState::Fault;
            break;
        case PpsState::Fault:
            // Fault: Error detected, motor stopped for safety
            // PWM: Stop motor
            // Limit switch: No action, system halted
            // Remain in fault or reset
            break;
    }
}

PpsState Pps::getState() const {
    return state_;
}

} // namespace LBR
