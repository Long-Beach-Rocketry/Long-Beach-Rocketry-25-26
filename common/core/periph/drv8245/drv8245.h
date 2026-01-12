
/**
 * @file drv8245.h
 * @brief Motor driver interface for DRV8245
 * @author Bex Saw
 * @date 2026-09-01
 */

#pragma once
#include "pwm.h"   // Update with actual PWM header path
#include "encoder.h" // Update with actual encoder header path

namespace LBR {

    class Drv8245 {
    public:
        Drv8245(Pwm& pwm, Encoder& encoder);
        ~Drv8245(); 

        /**
        * @brief Initialize the DRV8245 motor driver
        * @note Initializes GPIOs, Encoder and PWM for motor control
        * @return true if initialization is successful, false otherwise
        */
        bool init();

        /**
        * @brief Set motor speed and direction
        * @param speed Speed value from -100 to 100 (negative for reverse)
        * @note PWM duty cycle at |speed|%, direction based on sign
        */
        void set_speed(int speed);

        /**
        * @brief Stop or enable the motor (PWM control)
        * @param enable true to enable, false to stop
        * @note PWM duty cycle to 0/1 and brakes/start the motor
        */
        void enable(bool enable);

        /** 
        * @brief Set motor direction (PWM direction)
        * @param forward true for forward, false for reverse
        * @note Sets direction pin accordingly
        */
        void setDirection(bool forward);

        /**
        * @brief Move motor a specific number of degrees at given speed (encoder feedback)
        * @param degrees Number of degrees to move (positive or negative)
        * @param speed Speed value from 0 to 100
        * @note Uses encoder feedback to move specified degrees
        */
        void moveDegrees(int degrees, int speed);

        /**
        * @brief Get current encoder ticks
        * @param ticks Reference to store current encoder ticks
        * @note Retrieves the current tick count from the encoder
        */
        void getTicks(int& ticks) const;

        /**
        * @brief Get driver status
        * @param status Variable to store status code
        * @return 0 for OK, nonzero for error
        */
        int get_status() const;

    private:
        Pwm& _pwm;
        Encoder& _encoder;

        bool _initialized = false;
        int _status = 0; // 0 = OK, nonzero = error codes
    };

    } // namespace LBR
    
