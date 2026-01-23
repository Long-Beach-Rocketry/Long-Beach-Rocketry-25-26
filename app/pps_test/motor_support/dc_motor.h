#pragma once
/**
 * @file dc_motor.h
 * @brief Abstract motor interface for DC motors + encoder feedback
 * @note Motor interface functions to control motor actions.
 * @author Bex Saw
 * @date 2025/12/31
 */

#include "drv_8245.h" // PPS motor driver 
#include "drv_8874.h" // Auger motor driver
//#include "encoder.h"  // Encoder interface (whatever yshi generic has made)

namespace LBR {

// I might make a template class to allow for different motor drivers type to be pass the constructor 
class Motor {
public:
	Motor(Drv8245& drv, Drv8874& drv2, Encoder& encoder);
	~Motor();

	/**
	 * @brief Initialize motor driver and encoder
	 * @return true if successful, false otherwise
	 */
	bool init();

    /**
	* @brief Stop or enable the motor (PWM control)
	* @param enable true to enable, false to stop
	* @note PWM duty cycle to 0/1 and brakes/start the motor
	*/
	void motorEnable(bool enable);

	/**
	* @brief Set motor speed and direction
	* @param speed Speed value from -100 to 100 (negative for reverse)
	* @note PWM duty cycle at |speed|%, direction based on sign
	*/
	void motorSpeed(int speed);

	/**
	* @brief Set motor direction (PWM direction)
	* @param forward true for forward, false for reverse
	* @note Sets direction pin accordingly
	*/
	void motorDirection(bool forward);

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
	int getStatus() const;

private:
	Drv8245& _drv;
	Drv8874& _drv2; 
	Encoder& _encoder;

	bool _initialized{false};
	int _status{0}; // 0 = OK, nonzero = error code
};

} // namespace LBR