#include "motor_if.h"

namespace LBR {

Motor::Motor(Drv8245& drv, Encoder& encoder)
	: _drv(drv), _encoder(encoder) {}

Motor::~Motor() {}

bool Motor::init() {
	// Initialize motor driver and encoder
	_initialized = true;	
	return true;
}

void Motor::motorEnable(bool enable) {
	// using _drv.methodName() to enable or disable motor
}

void Motor::motorSpeed(int speed) {
	// using _drv.methodName() to set speed and direction based on sign of speed
}

void Motor::motorDirection(bool forward) {
	// using _drv.methodName() to set direction pin
}

void Motor::moveDegrees(int degrees, int speed) {
	// using _encoder.(something) to get current ticks and move motor accordingly
}

int Motor::getTicks() const {
	// using _encoder.methodName() to get current ticks
	return 0; 
}

int Motor::getStatus() const {
	// Return status code (0 for OK, nonzero for error)
	return 0;
}

} // namespace LBR