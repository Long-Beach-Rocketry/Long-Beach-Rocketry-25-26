/**
 * @file servo_motor.h
 * @brief Generic servo motor interface
 * @author Yshi Blanco
 * @date 01/12/2026
 */

#pragma once
#include <cstdint>
#include "pwm.h"

namespace LBR
{

/**
 * @brief Collection of servo motor parameters including a PWM instance,
 *        the minimum pulse width, and the maximum pulse width in microseconds
 */
struct ServoMotorParams
{
    Pwm& pwm;
    uint16_t min_pulse_width;
    uint16_t max_pulse_width;
    uint16_t max_angle;
};

class ServoMotor
{
public:
    /**
     * @brief Servo Motor Contructor
     * @param params Struct containing PWM interface, pulse width info, and max angle of the servo.
     */
    explicit ServoMotor(const ServoMotorParams& params_);

    /**
     * @brief Servo motor hardware initializer
     * @return true if successful, false otherwise
     */
    bool init();

    /**
     * @brief Moves the servo motor to the desired angle
     * @return true if successful, false otherwise
     */
    bool set_angle(uint16_t angle);

private:
    Pwm& pwm;
    uint16_t min_pulse_width;
    uint16_t max_pulse_width;
    uint16_t max_angle;

    uint8_t calculate_duty_cycle(uint16_t angle);
};
}  // namespace LBR