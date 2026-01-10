#include "st_pwm.h"
#include "reg_helpers.h"

namespace LBR
{
namespace Stml4
{

// Default clock frequency
static constexpr uint32_t pclk_freq = 4000000;

// Bit lengths
static constexpr uint8_t TIM_CCMRx_OCxM_BitWidth = 3;
static constexpr uint8_t TIM_CR1_CMS_BitWidth = 2;
static constexpr uint8_t TIM_CR1_DIR_BitWidth = 1;

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
     * @note Timers 16, 17 have 1 channel
     *       Timer 15 has 2 channels
     *       Timers 1, 2, 3, 4, 5, 6, 8 have 4 channels
     */
    if ((_channel < 1) || (_channel > 4))
    {
        return false;
    }
    else if ((_base_addr == TIM16 || _base_addr == TIM17) && (_channel > 1))
    {
        return false;
    }
    else if ((_base_addr == TIM15) && (_channel > 2))
    {
        return false;
    }

    /**
     * Make sure PwmMode & PwmDir are valid settings according to channel
     * @note Timers 15, 16, 17 are only capable of edge-aligned mode
     * @note Timers 15, 16, 17 are not capable of downcounting
     */
    if ((_base_addr == TIM15 || _base_addr == TIM16 || _base_addr == TIM17) &&
        ((_settings.mode != PwmMode::EDGE_ALIGNED) ||
         _settings.dir != PwmDir::UPCOUNTING))
    {
        return false;
    }

    // Configure timer according to channel and settings
    switch (_channel)
    {
        case 1:
            // Configure channel as an output
            _base_addr->CCMR1 &= ~TIM_CCMR1_CC1S_Msk;

            // Configure output mode
            SetReg(&_base_addr->CCMR1,
                   static_cast<uint32_t>(_settings.output_mode),
                   TIM_CCMR1_OC1M_Pos, TIM_CCMRx_OCxM_BitWidth);

            // Set preload bit to buffer updates to capture/compare register
            _base_addr->CCMR1 |= TIM_CCMR1_OC1PE;

            // Enable capture/compare output
            _base_addr->CCER |= TIM_CCER_CC1E;
            break;
        case 2:
            _base_addr->CCMR1 &= ~TIM_CCMR1_CC2S_Msk;
            SetReg(&_base_addr->CCMR1,
                   static_cast<uint32_t>(_settings.output_mode),
                   TIM_CCMR1_OC2M_Pos, TIM_CCMRx_OCxM_BitWidth);
            _base_addr->CCMR1 |= TIM_CCMR1_OC2PE;
            _base_addr->CCER |= TIM_CCER_CC2E;
            break;
        case 3:
            _base_addr->CCMR2 &= ~TIM_CCMR2_CC3S_Msk;
            SetReg(&_base_addr->CCMR2,
                   static_cast<uint32_t>(_settings.output_mode),
                   TIM_CCMR2_OC3M_Pos, TIM_CCMRx_OCxM_BitWidth);
            _base_addr->CCMR2 |= TIM_CCMR2_OC3PE;
            _base_addr->CCER |= TIM_CCER_CC3E;
            break;
        case 4:
            _base_addr->CCMR2 &= ~TIM_CCMR2_CC4S_Msk;
            SetReg(&_base_addr->CCMR2,
                   static_cast<uint32_t>(_settings.output_mode),
                   TIM_CCMR2_OC4M_Pos, TIM_CCMRx_OCxM_BitWidth);
            _base_addr->CCMR2 |= TIM_CCMR2_OC4PE;
            _base_addr->CCER |= TIM_CCER_CC4E;
            break;
        default:
            return false;
    }

    /**
     * Set MOE bit for Timers 1, 8, 15, 16, 17
     * @note Timer outputs are gated by the MOE bit since they contain
     *       functionality to handle break faults
     */
    if (_base_addr == TIM1 || _base_addr == TIM8 || _base_addr == TIM15 ||
        _base_addr == TIM16 || _base_addr == TIM17)
    {
        _base_addr->BDTR |= TIM_BDTR_MOE;
    }

    // Configure PWM mode and direction
    SetReg(&_base_addr->CR1, static_cast<uint32_t>(_settings.mode),
           TIM_CR1_CMS_Pos, TIM_CR1_CMS_BitWidth);
    SetReg(&_base_addr->CR1, static_cast<uint32_t>(_settings.dir),
           TIM_CR1_DIR_Pos, TIM_CR1_DIR_BitWidth);

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
        default:
            return false;
    }

    _curr_duty_cycle = duty_cycle;

    return true;
}

}  // namespace Stml4
}  // namespace LBR