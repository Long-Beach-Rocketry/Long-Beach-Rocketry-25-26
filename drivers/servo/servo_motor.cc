/**
 * @file servo_motor.cc
 * @brief Generic servo motor implementation
 */

#include "servo_motor.h"

namespace LBR
{

ServoMotor::ServoMotor(const ServoMotorParams& params)
    : pwm{params.pwm},
      min_pulse_width{params.min_pulse_width},
      max_pulse_width{params.max_pulse_width},
      max_angle{params.max_angle},
      frequency{params.frequency},
      period{frequency == 0 ? 0U : 1000000U / frequency},
      current_angle{0}
{
}

bool ServoMotor::init()
{
    initialized = false;
    if (frequency == 0 || period == 0)
    {
        return false;
    }

    if (min_pulse_width >= max_pulse_width)
    {
        return false;
    }

    if (max_angle == 0)
    {
        return false;
    }

    if (max_pulse_width > period)
    {
        return false;
    }

    if (!pwm.set_freq(frequency))
    {
        return false;
    }

    if (!pwm.init())
    {
        return false;
    }

    initialized = true;
    return true;
}

bool ServoMotor::set_angle(uint16_t angle)
{
    if (!initialized)
    {
        return false;
    }
    if (angle > max_angle)
    {
        return false;
    }

    const uint8_t duty_cycle = calculate_duty_cycle(angle);

    if (!pwm.set_duty_cycle(duty_cycle))
    {
        return false;
    }

    current_angle = angle;
    return true;
}

uint8_t ServoMotor::calculate_duty_cycle(uint16_t angle) const
{
    // Linearly map the angle to a pulse width, then convert it to a
    // duty-cycle percentage: (pulse width / PWM period) * 100.
    const uint32_t pulse_range = (max_pulse_width) - (min_pulse_width);

    const uint32_t pulse_width =
        (min_pulse_width) + (pulse_range * angle) / max_angle;

    const uint32_t duty_cycle = (pulse_width * 100U + period / 2U) / period;

    return static_cast<uint8_t>(duty_cycle);
}

uint16_t ServoMotor::get_angle() const
{
    return current_angle;
}

}  // namespace LBR