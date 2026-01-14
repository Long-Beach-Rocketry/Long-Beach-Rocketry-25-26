#include "servo_motor.h"

// Constants
static constexpr uint32_t OPERATING_FREQ_HZ = 50;
static constexpr uint32_t OPERATING_PERIOD_MICROSEC = 20000;

namespace LBR
{

ServoMotor::ServoMotor(const ServoMotorParams& params)
    : _pwm{params.pwm},
      _min_pulse_width{params.min_pulse_width},
      _max_pulse_width{params.max_pulse_width},
      _max_angle{params.max_angle}
{
}

bool ServoMotor::init()
{
    bool ret{true};
    ret = ret && _pwm.set_freq(OPERATING_FREQ_HZ);
    return ret &&
           _pwm.set_duty_cycle(calculate_duty_cycle(0));  // Start at 0 degrees
}

bool ServoMotor::set_angle(uint16_t angle)
{
    // Make sure desired angle is within range
    if ((angle < 0) || (angle > _max_angle))
    {
        return false;
    }

    return _pwm.set_duty_cycle(calculate_duty_cycle(angle));
}

uint8_t ServoMotor::calculate_duty_cycle(uint16_t angle)
{
    // Calculating microseconds per degree
    float us_per_degree =
        (_max_pulse_width - _min_pulse_width) / static_cast<float>(_max_angle);

    // Calculating pulse width in microseconds
    float pulse_width = (angle * us_per_degree) + _min_pulse_width;

    // Returning duty cycle
    return static_cast<uint8_t>((pulse_width / OPERATING_PERIOD_MICROSEC) *
                                100.0f);
}

}  // namespace LBR