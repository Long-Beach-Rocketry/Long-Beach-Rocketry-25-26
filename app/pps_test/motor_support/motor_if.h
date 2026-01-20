#pragma once
/**
 * @file motor_if.h
 * @brief Abstract motor interface for PPS
 * @note Motor interface functions to control motor actions.
 * @author Bex Saw
 * @date 2025/12/31
 */

/* Small note for the motor interface. This will be generic for a DC motor! 
This means that none of this should be specific to the PPS. The drilling app will probably use this as well. - TJ */

#include "drv_8245.h" // Motor driver 
//#include "encoder.h"  // Encoder interface (whatever yshi generic has made)

namespace LBR {

// I might make a template class to allow for different motor drivers type to be pass the constructor 
class Motor {
public:
	Motor(Drv8245& drv, Encoder& encoder);
	~Motor();

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
	Encoder& _encoder;

	bool _initialized{false};
	int _status{0}; // 0 = OK, nonzero = error code
};

} // namespace LBR