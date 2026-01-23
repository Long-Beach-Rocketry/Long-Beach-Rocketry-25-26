/**
* @file drv8874.h
* @brief DRV8874 motor driver header file
* @author Bex Saw 
* @date 01/21/2026
*/

#pragma once
#include <cstdint>
#include "gpio.h"
#include "pwm.h"

namespace LBR
{

class Drv8874
{
public:
    enum class Direction
    {
        FORWARD,
        REVERSE
    };

    /** 
    * @brief Constructor for DRV8874 motor driver
    * @param dir GPIO pin for direction control - PH/IN2
    * @param pwm PWM channel for motor speed contorl (EN/IN1) - AGR_PWM_P
    * @param sleep GPIO pin for sleep control - nSLEEP
    * @param fault GPIO pin for fault indication - NFAULT
    */
    Drv8874(Gpio& dir, Pwm& pwm, Gpio& sleep, Gpio& fault);

    /**
    * @brief Initialize the DRV8874 motor driver
    * @note Initialize GPIO + PWM settings
    */
    void init();

    /**
    * @brief Set motor speed 
    * @param pwm_value PWM value to set motor speed (-max to +max)
    * @note This function set the auger_board motor driver board speed 
    */
    void setSpeed(int16_t pwm_value);

    /**
    * @brief set motor direction
    * @param direction Direction enum (FORWARD or REVERSE)
    * @note This function set the auger_board motor driver board direction
    */
    void setDirection(Direction direction);

    /**
    * @brief Enable coast mode
    * @note Set the auger_board motor driver to Hi-Z state
    */
    void enableCoast();

    /**
    * @brief Enable brake mode
    * @note Set the auger_board motor driver to brake state
    */
    void enableBrake();

    /**
    * @brief Set sleep mode
    * @note set the auger_board motor driver to sleep mode
    * @param enable true to enable sleep mode, false to disable
    */
    void setSleep(bool enable);

    /**
    * @brief Check for fault condition
    * @return true if fault condition is detected, false otherwise
    */
    bool checkFault() const;

private:
    Gpio& dir_;
    Pwm& pwm_;
    Gpio& sleep_;
    Gpio& fault_;
};
}  // namespace LBR