/**
* @file drv8245.h
* @brief DRV8245 motor driver header file
* @author Bex Saw
* @date 1/15/2026
*/

/**
 * @class Drv8245
 * @brief DRV8245 Motor Driver (HW variant, GPIO/PWM control)
 *
 * @note To myself: This class provides a portable, hardware interface for the DRV8245-H motor driver.
 * All hardware mapping is handled by the BSP; this class only uses abstracted Gpio and Pwm objects.
 */
#pragma once
#include <cstdint>
#include "gpio.h"
#include "pwm.h"

namespace LBR
{

class Drv8245
{
public:
    enum class Direction : uint8_t
    {
        Forward = 1,
        Reverse = 0
    };

    /**
     * @brief Construct a new Drv8245 object
     * @param dir Direction (PH/IN2) GPIO
     * @param pwm PWM (EN/IN1) PWM channel
     * @param drv_z DRVZ (coast/Hi-Z) GPIO
     * @param sleep nSLEEP (sleep/wake) GPIO
     * @param fault nFAULT (fault input) GPIO
     */
    Drv8245(Gpio& dir, Pwm& pwm, Gpio& drv_z, Gpio& sleep, Gpio& fault);

    /**
    * @brief Initialize the motor driver
    * @note This function sets up the GPIO and PWM interfaces.
    */
    void init();

    /**
    * @brief Set motor speed (0-100%)
    * @param pwm_value Speed value from 0 (stop) to 100 (full speed)
    * @note This function sets the motor speed by adjusting the PWM duty cycle.
    */
    void setSpeed(uint16_t pwm_value);

    /**
    * @brief set motor direction
    * @param dir Direction enum (Forward/Reverse)
    * @note This function sets the deploy_board motor direction by controlling the DIR pin.
    */
    void setDirection(Direction dir);

    /**
    * @brief Enable coast (Hi-Z) mode
    * @note This function disables the deploy_board motor outputs, putting them in a high-impedance state.
    */
    void enableCoast();

    /**
    * @brief Enable/Disable sleep mode
    * @note set deploy_board motor into a low-power sleep state when enabled, and wakes it when disabled.
    */
    void setSleep(bool enable);

    /**
    * @brief Check for fault condition
    * @return true if a fault is detected, false otherwise
    */
    bool checkFault() const;

private:
    Gpio& dir_;
    Pwm& pwm_;
    Gpio& drv_z_;
    Gpio& sleep_;
    Gpio& fault_;
};
}  // namespace LBR
