#pragma once
/**
 * @file pps_helpers.h
 * @brief PPS-specific motor helper functions.
 * @note These functions are specific to the PPS application and should not be part of the generic DC motor interface.
 *
 * @author Bex Saw
 * @date 2026/01/13
 */
#include "dc_motor.h" 

/**
 * @brief Move the motor in the deploy direction (extend mechanism).
 * @note Used during the Deploy state to drive the mechanism until the deployed limit switch is reached.
 * TODO: Implement PWM logic for deployment.
 */
void motorDeploy();

/**
 * @brief Move the motor to the target (dig) position.
 * @note Used during the Rotate state to position the mechanism for drilling.
 * TODO: Implement PWM logic for rotation.
 */
void motorTarget();

/**
 * @brief Move the motor in the retract direction (retract mechanism).
 * @note Used during the Retract state to drive the mechanism until the retracted limit switch is reached.
 * TODO: Implement PWM logic for retraction.
 */
void motorRetract();
