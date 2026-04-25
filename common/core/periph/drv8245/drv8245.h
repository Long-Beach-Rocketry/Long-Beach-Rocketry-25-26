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
    * @note This is enough for the DRV8245, which only requires a direction pin and a PWM pin
    *       And motion control through PID controller loops to compensate for error gains
    */
    struct DrvParams
    {
        Gpio& dir;
        Pwm& pwm;
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

    // Store current speed since Pwm interface doesn't have a getter
    uint8_t current_speed = 0;
};
}  // namespace LBR
