#include "st_pwm.h"

namespace LBR
{
namespace Stml4
{

// Default clock frequency
static constexpr uint32_t pclk_freq = 4000000;

HwPwm::HwPwm(const StPwmParams& params)
    : _base_addr{params.base_addr},
      _channel{params.channel},
      _settings{params.settings}
{
}

bool HwPwm::init()
{
    if (_base_addr == nullptr)
    {
        return false;
    }

    // Timers 6/7 are not capable of PWM
    if (_base_addr == TIM6 || _base_addr == TIM7)
    {
        return false;
    }

    switch (_channel)
    {
        case 1:

            break;
        case 2:

            break;
        case 3:

            break;
        case 4:

            break;
        case 5:

            break;
        case 6:

            break;
        default:
            return false;
    }

    // Initialize counter and update registers
    _base_addr->EGR |= TIM_EGR_UG;

    // Enable auto-reload preload register and enable counter
    _base_addr->CR1 |= (TIM_CR1_ARPE | TIM_CR1_CEN);

    return true;
}

bool HwPwm::set_freq(uint32_t freq)
{

    return true;
}

bool HwPwm::set_duty_cycle(float duty_cycle)
{

    return true;
}

}  // namespace Stml4
}  // namespace LBR