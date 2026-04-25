#include "drv8245.h"

namespace LBR
{

Drv8245::Drv8245(const DrvParams& params)
    : dir_pin(params.dir), pwm_pin(params.pwm)
{
}

void Drv8245::set_direction(Direction dir)
{
    if (dir == Direction::FORWARD)
    {
        this->dir_pin.set(1);
    }
    else
    {
        this->dir_pin.set(0);
    }
}

void Drv8245::set_speed(uint8_t speed)
{
    pwm_pin.set_duty_cycle(speed);
    current_speed = speed;
}

uint8_t Drv8245::get_speed() const
{
    return current_speed;
}

Drv8245::State Drv8245::get_state() const
{
    return {dir_pin.read(), current_speed};
}

}  // namespace LBR