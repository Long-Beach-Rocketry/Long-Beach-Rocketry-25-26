/**
 * @file pps.h
 * @brief PPS module interface
 * @note This module manages the positioning system state machine.
 * @author Bex Saw
 * @date 12/31/2025
 */

#pragma once
#include "bsp/motor_if.h"
#include <cstdint>

// PPS State Machine States
enum class PpsState {
    Idle,
    Homing,
    MovingToTarget,
    AtEndstop,
    Fault
};

struct PpsContext {
    PpsState state = PpsState::Idle;

    // IMU data field (quaternion)
    
    // PWM control field (duty cycle)

    // GPIO/limit switch states

    // etc.
};

void ppsInit(PpsContext& ctx);
void ppsUpdate(PpsContext& ctx);
