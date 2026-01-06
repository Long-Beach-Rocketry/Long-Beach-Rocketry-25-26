#include "st_pwm.h"
#include "reg_helpers.h"

namespace LBR
{
namespace Stml4
{

// Default clock frequency
static constexpr uint32_t pclk_freq = 4000000;

HwPwm::HwPwm(const StPwmParams& params)
    : _base_addr{params.base_addr},
      _channel{params.channel},
      _settings{params.settings},
      _curr_freq{pclk_freq},
      _curr_duty_cycle{100.0}
{
}

bool HwPwm::init()
{
    if (_base_addr == nullptr)
    {
        return false;
    }

    // Timers 6/7 are not capable of PWM
    if ((_base_addr == TIM6) || (_base_addr == TIM7))
    {
        return false;
    }

    /**
     * Make sure channel number is valid according to given timer
     * @note Timers 15, 16, 17 have 2 channels
     *       Timers 2, 3, 4, 5 have 4 channels
     *       Timers 1, 8 have 6 channels
     */
    if ((_channel < 1) || (_channel > 6))
    {
        return false;
    }
    else if ((_base_addr == TIM15 || _base_addr == TIM16 ||
              _base_addr == TIM17) &&
             (_channel > 2))
    {
        return false;
    }
    else if ((_base_addr == TIM2 || _base_addr == TIM3 || _base_addr == TIM4 ||
              _base_addr == TIM5) &&
             (_channel > 4))
    {
        return false;
    }

    // Configure timer according to channel and settings
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
    // Make sure counter was initialized
    if (!(_base_addr->CR1 & TIM_CR1_CEN_Msk))
    {
        return false;
    }

    if ((freq < 1) || (freq > pclk_freq))
    {
        return false;
    }

    uint32_t reload_val = (pclk_freq / freq) - 1;

    _base_addr->ARR = reload_val;
    _curr_freq = freq;

    // Update auto-reload register according to new freq
    return set_duty_cycle(_curr_duty_cycle);
}

bool HwPwm::set_duty_cycle(float duty_cycle)
{
    // Make sure counter was initialized
    if (!(_base_addr->CR1 & TIM_CR1_CEN_Msk))
    {
        return false;
    }

    if ((duty_cycle < 0.0) || (duty_cycle > 100.0))
    {
        return false;
    }

    uint32_t compare_val =
        static_cast<uint32_t>((duty_cycle / 100.0) * (_base_addr->ARR));

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

    _curr_duty_cycle = duty_cycle;

    return true;
}

}  // namespace Stml4
}  // namespace LBR