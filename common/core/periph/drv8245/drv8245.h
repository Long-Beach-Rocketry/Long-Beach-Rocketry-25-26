/**
* @file drv8245.h
* @brief DRV8245 Motor Driver Interface for PPS
* @author Bex Saw
* @date 2026-09-01
*/

#pragma once

// DRV8245 Motor Driver Interface for PPS

// Initialize the motor driver hardware
void drv8245_init(void);

// Set motor speed and direction (speed: -100 to 100, negative for reverse)
void drv8245_set_speed(int speed);

// Stop the motor
void drv8245_stop(void);

// Optional: Get driver status (returns 0 for OK, nonzero for error)
int drv8245_get_status(void);

// Optional: Enable/disable motor driver (power control)
void drv8245_enable(bool enable);

// Optional: Move motor by degrees (uses encoder feedback)
void drv8245_move_degrees(int degrees);

// Optional: Get encoder ticks
int drv8245_get_ticks(void);
