#pragma once
/**
 * @file motor_if.h
 * @brief Abstract motor interface for PPS
 * @note Motor interface functions to control motor actions.
 * @author Bex Saw
 * @date 2025/12/31
 */
 
// #include "gpio.h" // Limit switch reading
// #include "drv_8255.h" // PWM + ENCODER driver

/**
 * @brief Stop all motor movement immediately.
 * @note Used to halt the mechanism after reaching a limit switch or completing an action.
 * Example: Called after deployment or rotation is complete.
 */
void motorStop();

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