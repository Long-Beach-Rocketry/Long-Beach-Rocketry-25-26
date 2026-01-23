#include "drv8874.h"
#include <cmath>

namespace LBR
{

Drv8874::Drv8874(Gpio& dir, Pwm& pwm, Gpio& sleep, Gpio& fault)
    : dir_(dir), pwm_(pwm), sleep_(sleep), fault_(fault)
{
}

void Drv8874::init()
{
    sleep_.set(true);  // Wake up driver
}

void Drv8874::setSpeed(int16_t pwm_value)
{
    if (pwm_value > 100)
    {
        pwm_value = 100;  // Cap at 100%
    }
    else if (pwm_value < -100)
    {
        pwm_value = -100;  // Cap at -100%
    }
    pwm_.set_duty_cycle(static_cast<uint16_t>(std::abs(pwm_value)));
}

void Drv8874::setDirection(Direction direction)
{
    if (direction == Direction::FORWARD)
    {
        dir_.set(true);
    }
    else
    {
        dir_.set(false);
    }
}

void Drv8874::enableCoast()
{
    sleep_.set(false);  // Enter sleep mode
}

void Drv8874::enableBrake()
{
    sleep_.set(true);          // Wake up
    pwm_.set_duty_cycle(100);  // Set PWM to max for braking
}

void Drv8874::setSleep(bool enable)
{
    if (enable)
    {
        sleep_.set(true);  // Wake up
    }
    else
    {
        sleep_.set(false);  // Enter sleep mode
    }
}

bool Drv8874::checkFault() const
{
    return !fault_.read();  // Active low fault signal
}
}  // namespace LBR