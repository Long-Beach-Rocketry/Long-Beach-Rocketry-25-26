#include "drv8245.h"

namespace LBR
{

Drv8245::Drv8245(Gpio& dir, Pwm& pwm, Gpio& drv_z, Gpio& sleep, Gpio& fault)
    : dir_(dir), pwm_(pwm), drv_z_(drv_z), sleep_(sleep), fault_(fault)
{
}

void Drv8245::init()
{
    drv_z_.set(true);  // Enable driver
    sleep_.set(true);  // Wake up driver
}

void Drv8245::setSpeed(uint16_t pwm_value)
{
    if (pwm_value > 100)
    {
        pwm_value = 100;  // Cap at 100%
    }
    pwm_.set_duty_cycle(pwm_value);
}

void Drv8245::setDirection(Direction dir)
{
    if (dir == Direction::Forward)
    {
        dir_.set(true);
    }
    else
    {
        dir_.set(false);
    }
}

void Drv8245::enableCoast()
{
    drv_z_.set(false);  // Disable driver outputs (Hi-Z)
}

void Drv8245::setSleep(bool enable)
{
    if (enable)
    {
        sleep_.set(false);  // Enter sleep mode
    }
    else
    {
        sleep_.set(true);  // Wake up
    }
}

bool Drv8245::checkFault() const
{
    return !fault_.read();  // Active low nFAULT
}

}  // namespace LBR