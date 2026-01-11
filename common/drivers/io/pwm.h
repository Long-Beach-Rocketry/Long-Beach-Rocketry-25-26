/**
 * @file pwm.h
 * @brief PWM driver interface.
 * @author Yshi Blanco
 * @date 12/24/2025
 */

#pragma once
#include <cstdint>

namespace LBR
{

/**
 * @class Pwm
 * @brief General PWM interface
 */
class Pwm
{
public:
    /**
     * @brief Sets the frequency of the PWM timer
     * @param freq Desired frequency in Hz
     * @return true if successful, false otherwise
     */
    virtual bool set_freq(uint32_t freq) = 0;

    /**
     * @brief Sets the duty cycle for the PWM peripheral
     * @param duty_cycle Desired duty cycle as a percentage
     * @return true if successful, false otherwise
     */
    virtual bool set_duty_cycle(uint8_t duty_cycle) = 0;

    ~Pwm() = default;
};
}  // namespace LBR