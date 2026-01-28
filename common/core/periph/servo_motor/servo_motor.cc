#include "servo_motor.h"

// Constants
static constexpr uint32_t OPERATING_FREQ_HZ = 50;
static constexpr uint32_t OPERATING_PERIOD_MICROSEC = 20000;

namespace LBR
{

ServoMotor::ServoMotor(const ServoMotorParams& params_)
    : pwm{params_.pwm},
      min_pulse_width{params_.min_pulse_width},
      max_pulse_width{params_.max_pulse_width},
      max_angle{params_.max_angle}
{
}

bool ServoMotor::init()
{
    bool ret{true};
    ret = ret && pwm.set_freq(OPERATING_FREQ_HZ);
    return ret &&
           pwm.set_duty_cycle(calculate_duty_cycle(0));  // Start at 0 degrees
}

bool ServoMotor::set_angle(uint16_t angle)
{
    // Make sure desired angle is within range
    if ((angle < 0) || (angle > max_angle))
    {
        return false;
    }

    return pwm.set_duty_cycle(calculate_duty_cycle(angle));
}

uint8_t ServoMotor::calculate_duty_cycle(uint16_t angle)
{
    // Calculating microseconds per degree
    float us_per_degree =
        (max_pulse_width - min_pulse_width) / static_cast<float>(max_angle);

    // Calculating pulse width in microseconds
    float pulse_width = (angle * us_per_degree) + min_pulse_width;

    // Returning duty cycle
    return static_cast<uint8_t>((pulse_width / OPERATING_PERIOD_MICROSEC) *
                                100.0f);
}

}  // namespace LBR