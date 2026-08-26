#include "drv8245.h"
#include "delay.h"

namespace LBR
{
    
Drv8245::Drv8245(const DrvParams& params)
    : dir_pin(params.dir),
      pwm_pin(params.pwm),
      nsleep_pin(params.nsleep),
      drvoff_pin(params.drvoff)
{
}

void Drv8245::init()
{
    // Keep outputs Hi-Z while bringing the device out of SLEEP
    drvoff_pin.set(1);

    // Wake: nSLEEP high starts the SLEEP -> STANDBY transition
    nsleep_pin.set(1);

    // Acknowledge the wake-up with a reset pulse: nSLEEP low for tRESET,
    // then back high before tSLEEP min elapses (or the device re-enters SLEEP)
    nsleep_pin.set(0);
    nsleep_pin.set(1);

    // Wait for the device to be ready to accept PH/EN control
    // Enable the bridge outputs
    drvoff_pin.set(0);
}

void Drv8245::sleep()
{
    nsleep_pin.set(0);
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