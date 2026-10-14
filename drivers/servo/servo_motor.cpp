/**
 * @file servo_motor.cpp
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
      period{frequency == 0 ? 0U : 1000000U / frequency}
{
}

bool ServoMotor::init()
{

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

    return set_angle(0);
}

bool ServoMotor::set_angle(uint16_t angle)
{
    if (angle > max_angle)
    {
        return false;
    }

    const uint8_t duty_cycle = calculate_duty_cycle(angle);

    return pwm.set_duty_cycle(duty_cycle);
}

uint8_t ServoMotor::calculate_duty_cycle(uint16_t angle) const
{
    const uint32_t pulse_range = static_cast<uint32_t>(max_pulse_width) -
                                 static_cast<uint32_t>(min_pulse_width);

    const uint32_t pulse_width = static_cast<uint32_t>(min_pulse_width) +
                                 (pulse_range * angle) / max_angle;

    const uint32_t duty_cycle = (pulse_width * 100U + period / 2U) / period;

    return static_cast<uint8_t>(duty_cycle);
}

}  // namespace LBR