#pragma once
/**
 * @file motor_if.h
 * @brief Abstract motor interface for PPS
 * @note Motor interface functions to control motor actions.
 * @author Bex Saw
 * @date 2025/12/31
 */
 
// #include "pwm.h" something

/**
* @brief Stop the motor.
*/
void motorStop();

/**
* @brief Move the motor to the home direction.
*/
void motorHomeDir();

/**
* @brief Move the motor to the target direction.
*/
void motorTargetDir();
