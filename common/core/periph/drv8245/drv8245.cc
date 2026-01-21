#include "drv8245.h"

namespace LBR {

Drv8245::Drv8245(Gpio& dir, Pwm& pwm, Gpio& drv_z, Gpio& sleep, Gpio& fault)
    : dir_(dir), pwm_(pwm), drv_z_(drv_z), sleep_(sleep), fault_(fault)
{
}

void Drv8245::init() {
    drv_z_.set(true); // Enable driver
    sleep_.set(true);  // Wake up driver
}

void Drv8245::setPwm(Direction dir, uint16_t pwm_value) {
    dir_.set(dir == Direction::Forward ? true : false);
    pwm_.set_duty_cycle(static_cast<uint8_t>(pwm_value / 10)); // Convert 0-1000 to 0-100%
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
    return !fault_.read(); // Active low nFAULT
}

} // namespace LBR