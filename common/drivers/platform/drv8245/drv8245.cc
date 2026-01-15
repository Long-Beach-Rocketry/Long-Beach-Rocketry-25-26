#include "drv8245.h"

namespace LBR {

Drv8245::Drv8245(Gpio& dir, Pwm& pwm, Gpio& drv_z, Gpio& sleep, Gpio& fault)
    : dir_(dir), pwm_(pwm), drv_z_(drv_z), sleep_(sleep), fault_(fault)
{
}

void Drv8245::init() {
    dir_.init();
    drv_z_.init();
    sleep_.init();
    fault_.init();
    pwm_.init();
    drv_z_.set(true); // Enable driver
    sleep_.set(true);  // Wake up driver
}

void Drv8245::setPwm(Direction dir, uint16_t pwm_value) {
    dir_.set(dir == Direction::Forward ? true : false);
    pwm_.setDutyCycle(pwm_value); // will change based on yb-pwm interface
}

void Drv8245::enableCoast() {
    drv_z_.set(false); // Disable driver outputs (Hi-Z)
}

void Drv8245::setSleep(bool enable) {
    if (enable) {
        sleep_.set(false); // Enter sleep mode
    } else {
        sleep_.set(true);  // Wake up
    }
}

bool Drv8245::checkFault() const {
    return !fault_.get(); // Active low nFAULT
}

} // namespace LBR