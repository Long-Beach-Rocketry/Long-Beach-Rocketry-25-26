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
#include "gpio.h"
#include "pwm.h"
#include <cstdint>

namespace LBR {

class Drv8245 {
public:

    enum class Direction : uint8_t {
        Forward = 1,
        Reverse = 0
    };

    /**
     * @brief Construct a new Drv8245 object
     * @param dir Direction (PH/IN2) GPIO
     * @param pwm PWM (EN/IN1) PWM channel
     * @param drvoff DRVOFF (coast/Hi-Z) GPIO
     * @param sleep nSLEEP (sleep/wake) GPIO
     * @param fault nFAULT (fault input) GPIO
     */
    Drv8245(Gpio& dir, Pwm& pwm, Gpio& drvoff, Gpio& sleep, Gpio& fault);

    /**
    * @brief Initialize the motor driver
    * @note This function sets up the GPIO and PWM interfaces.
    */
    void init();

    /**
    * @brief Set motor PWM and direction
    * @param dir Direction enum (Forward/Reverse)
    * @param pwm_value PWM duty cycle (0-1000 for 0%-100%)
    */
    void setPwm(Direction dir, uint16_t pwm_value);

    /**
    * @brief Enable coast (Hi-Z) mode
    * @note This function disables the motor outputs, putting them in a high-impedance state.
    */
    void enableCoast();
    
    /**
    * @brief Enable/Disable sleep mode
    * @note set motor into a low-power sleep state when enabled, and wakes it when disabled.
    */
    void setSleep(bool enable);

    /**
    * @brief Check for fault condition
    * @return true if a fault is detected, false otherwise
    */
    bool checkFault() const;

    // bool currentSense(float& current) const; 
    // Optional: Implement if current sensing is needed (ADC raw)

private:
    Gpio& dir_;
    Pwm& pwm_;
    Gpio& drvoff_;
    Gpio& sleep_;
    Gpio& fault_;
}; 
} // namespace LBR
  