/**
 * @file servo_motor.h
 * @brief Generic servo motor interface
 */

#pragma once

#include <cstdint>
#include "pwm.h"

namespace LBR
{

struct ServoMotorParams
{
    Pwm& pwm;
    uint16_t min_pulse_width;
    uint16_t max_pulse_width;
    uint16_t max_angle;
    uint32_t frequency;
};

class ServoMotor
{
public:
    /**
     * @brief Creates a servo motor driver
     *
     * @param params Servo PWM and angle configuration
     */
    explicit ServoMotor(const ServoMotorParams& params);

    /**
     * @brief Initializes the servo PWM
     *
     * @return true if successful, false otherwise
     */
    bool init();

    /**
     * @brief Commands the servo to an angle
     *
     * @param angle Desired angle from 0 to max_angle
     * @return true if successful, false otherwise
     */
    bool set_angle(uint16_t angle);

    /**
     * @brief Gets the current angle of the servo
     *
     * @return Current angle in degrees
     */
    uint16_t get_angle() const;

private:
    Pwm& pwm;
    uint32_t min_pulse_width;
    uint32_t max_pulse_width;
    uint16_t max_angle;
    uint32_t frequency;
    uint32_t period;
    uint16_t current_angle;
    bool initialized = false;

    /**
     * @brief Calculates the PWM duty cycle for a given servo angle.
     * @param angle Servo angle in degrees.
     * @return Duty cycle as a percentage.
     */
    uint8_t calculate_duty_cycle(uint16_t angle) const;
};

}  // namespace LBR