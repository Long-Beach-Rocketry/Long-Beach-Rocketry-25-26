/**
* @file drv8245.h
* @brief DRV8245 motor driver header file
* @author Bex Saw
* @date 1/15/2026
*/

#pragma once
#include <cstdint>
#include "gpio.h"
#include "pwm.h"

namespace LBR
{

/**
 * @class Drv8245
 * @brief DRV8245 Motor Driver (HW variant, GPIO/PWM control)
 *
 */
class Drv8245
{
public:
    /**
    * @brief Direction enum for motor control
    */
    enum class Direction : uint8_t
    {
        FORWARD = 1,
        REVERSE = 0
    };

    /// MODE: we usnig PH/EN mode, so we only need two pins: one for direction (PHASE) and one for PWM (ENABLE DIR)

    /**
    * @brief Struct to hold parameters for Drv8245 initialization
    * @note dir/pwm drive PH/IN2 and EN/IN1. nsleep and drvoff are the device's mandatory
    *       wake and output-enable control pins (HW variant) - see init()/sleep().
    */
    struct DrvParams
    {
        Gpio& dir;
        Pwm& pwm;
        Gpio& nsleep;
        Gpio& drvoff;
    };

    struct State
    {
        bool direction;  // true for forward, false for reverse
        uint8_t speed;   // PWM duty cycle (0-255)
    };

    /**
     * @brief Constructor for Drv8245
     * @param params Reference to the initialization parameters
     */
    explicit Drv8245(const DrvParams& params);

    /**
     * @brief Wake the device from SLEEP and enable its outputs
     * @note Must be called (and complete) before set_direction()/set_speed() have any effect.
     *       Runs the mandatory nSLEEP wake + reset-pulse handshake required by the HW variant,
     *       then drives DRVOFF low to bring the bridge into the ACTIVE state.
     */
    void init();

    /**
     * @brief Put the device into low-power SLEEP state (outputs Hi-Z)
     */
    void sleep();

    /**
     * @brief Set the motor direction
     * @param dir Direction of motor rotation
     * @note Direction ENable
     */
    void set_direction(Direction dir);

    /**
     * @brief Set the motor speed (PWM duty cycle)
     * @param speed Speed of motor rotation (0-255)
     */
    void set_speed(uint8_t speed);

    /**
    * @brief Get the current speed of the motor
    */
    uint8_t get_speed() const;

    /**
    * @brief Get the current state of the motor (direction and speed)
    * @return Current state as a struct containing direction and speed
    */
    State get_state() const;

private:
    Gpio& dir_pin;
    Pwm& pwm_pin;
    Gpio& nsleep_pin;
    Gpio& drvoff_pin;

    // Store current speed since Pwm interface doesn't have a getter
    uint8_t current_speed = 0;
};
}  // namespace LBR
