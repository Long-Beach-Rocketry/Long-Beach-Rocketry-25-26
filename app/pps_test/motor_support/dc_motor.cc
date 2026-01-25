#include "dc_motor.h"
#include <cstdlib>
#include <algorithm>

namespace LBR {

Motor::Motor(Drv8245& drv)
	: _drv(drv) {}

Motor::~Motor() {}

bool Motor::init() {
	// Initialize driver
	_drv.init();
	_initialized = true;
	return true;
}

void Motor::motorEnable(bool enable) {
	if (enable) {
		_drv.setSleep(false);  // Wake up driver (enable)
	} else {
		_drv.enableCoast();    // Set driver to Hi-Z (disable)
	}
}

void Motor::motorSpeed(int speed) {
	// Clamp speed to -100 to 100
	speed = std::clamp(std::abs(speed), 0, 100);
	_drv.setSpeed(static_cast<uint16_t>(speed));
}

void Motor::motorDirection(bool forward) {
    _drv.setDirection(forward ? Drv8245::Direction::Forward : Drv8245::Direction::Reverse);
}

void Motor::moveDegrees(int degrees, int speed) {
	// using _encoder.(something) to get current ticks and move motor accordingly
}

int Motor::getTicks() const {
	// No encoder implemented yet
	return 0;
}

int Motor::getStatus() const {
	// Return status code (0 for OK, nonzero for error)
	if( !_initialized) {
		return -1; // Not initialized
	}
	if (_drv.checkFault()) {
		return -2; // Driver fault
	}
	/* if == (_encoder.getStatus() != 0) {
		return -3; // Encoder error
	} */ 
	return 0;
}

} // namespace LBR