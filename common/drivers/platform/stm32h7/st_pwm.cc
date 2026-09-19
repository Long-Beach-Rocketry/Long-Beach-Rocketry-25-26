#include "st_pwm.h"
#include "reg_helpers.h"

namespace LBR
{
namespace Stmh7
{

static constexpr uint8_t kArrVal = 99;
static constexpr uint32_t kMaxPsc = 65536;  // 2^(16)

// Bit lengths
static constexpr uint8_t kTIM_CCMRx_OCxM_BitWidth = 4;
static constexpr uint8_t kTIM_CR1_CMS_BitWidth = 2;
static constexpr uint8_t kTIM_CR1_DIR_BitWidth = 1;
static constexpr uint8_t kTIM_CCRx_BitWidth = 16;

/**
 * Maxmimum PWM frequencies according to ARR value
 */
static inline uint32_t max_edge_aligned_freq(uint32_t pclk_freq)
{
    return pclk_freq / (kArrVal + 1);
}

static inline uint32_t max_center_aligned_freq(uint32_t pclk_freq)
{
    return pclk_freq / (2 * (kArrVal + 1));
}

// For specific General-Purpose Timers
static inline bool is_timer_12_to_17(TIM_TypeDef* t)
{
    return (TIM12 == t || TIM13 == t || TIM14 == t || TIM15 == t ||
            TIM16 == t || TIM17 == t);
}

HwPwm::HwPwm(const StPwmParams& params_)
    : base_addr{params_.base_addr},
      channel{params_.channel},
      settings{params_.settings},
      curr_freq{params_.pclk_freq},
      curr_duty_cycle{0},
      pclk_freq{params_.pclk_freq}
{
}

bool HwPwm::init()
{
    if (base_addr == nullptr)
    {
        return false;
    }

    base_addr->CR1 &= ~TIM_CR1_CEN;
    /* 
    * TIM 1,8 have 4 channels and can have any mode
    * TIM 2,3,4,5,23,24 have 4 channels and any mode
    * TIM 6.7 cannot do PWM
    * TIM 12,15 have 2 channels and edge up mode only
    * TIM 13, 14,16,17 have 1 channel and edge up mode only 
    */

    if (base_addr == TIM6 || base_addr == TIM7)
    {
        return false;
    }

    else if (is_timer_12_to_17(base_addr))
    {

        if (settings.mode != PwmMode::EDGE_ALIGNED ||
            static_cast<uint8_t>(channel) > 2)
        {
            return false;
        }

        else if (base_addr != TIM12 && base_addr != TIM15 &&
                 channel != PwmChannel::CH1)
        {
            return false;
        }
    }

    // CR1 Auto-reload, Mode, Direction
    base_addr->CR1 |= TIM_CR1_ARPE;
    SetReg(&base_addr->CR1, static_cast<uint32_t>(settings.mode),
           TIM_CR1_CMS_Pos, kTIM_CR1_CMS_BitWidth);
    if (settings.mode == PwmMode::EDGE_ALIGNED)
    {
        SetReg(&base_addr->CR1, static_cast<uint32_t>(settings.direction),
               TIM_CR1_DIR_Pos, kTIM_CR1_DIR_BitWidth);
    }

    // Channel setting
    switch (channel)
    {

        case PwmChannel::CH1:
            base_addr->CCMR1 &= ~TIM_CCMR1_CC1S;  // Channel x as Output
            SetReg(&base_addr->CCMR1,
                   static_cast<uint32_t>(settings.output_mode),
                   TIM_CCMR1_OC1M_Pos,
                   kTIM_CCMRx_OCxM_BitWidth);     // Config Output Mode
            base_addr->CCR1 = curr_duty_cycle;    // 0% Duty Cycle
            base_addr->CCMR1 |= TIM_CCMR1_OC1PE;  // Enable Preload
            base_addr->CCER |=
                TIM_CCER_CC1E;  // Enable the output for Channel x
            break;

        case PwmChannel::CH2:
            base_addr->CCMR1 &= ~TIM_CCMR1_CC2S;
            SetReg(&base_addr->CCMR1,
                   static_cast<uint32_t>(settings.output_mode),
                   TIM_CCMR1_OC2M_Pos, kTIM_CCMRx_OCxM_BitWidth);
            base_addr->CCR2 = curr_duty_cycle;
            base_addr->CCMR1 |= TIM_CCMR1_OC2PE;
            base_addr->CCER |= TIM_CCER_CC2E;
            break;

        case PwmChannel::CH3:
            base_addr->CCMR2 &= ~TIM_CCMR2_CC3S;
            SetReg(&base_addr->CCMR2,
                   static_cast<uint32_t>(settings.output_mode),
                   TIM_CCMR2_OC3M_Pos, kTIM_CCMRx_OCxM_BitWidth);
            base_addr->CCR3 = curr_duty_cycle;
            base_addr->CCMR2 |= TIM_CCMR2_OC3PE;
            base_addr->CCER |= TIM_CCER_CC3E;
            break;

        case PwmChannel::CH4:
            base_addr->CCMR2 &= ~TIM_CCMR2_CC4S;
            SetReg(&base_addr->CCMR2,
                   static_cast<uint32_t>(settings.output_mode),
                   TIM_CCMR2_OC4M_Pos, kTIM_CCMRx_OCxM_BitWidth);
            base_addr->CCR4 = curr_duty_cycle;
            base_addr->CCMR2 |= TIM_CCMR2_OC4PE;
            base_addr->CCER |= TIM_CCER_CC4E;
            break;

        default:
            return false;
    }

    /**
   * Set MOE bit for Timers 1, 8, 15, 16, 17
   * @note Timer outputs are gated by the MOE bit since they contain
   *       functionality to handle break faults
   */
    if (base_addr == TIM1 || base_addr == TIM8 || base_addr == TIM15 ||
        base_addr == TIM16 || base_addr == TIM17)
    {
        base_addr->BDTR |= TIM_BDTR_MOE;
    }

    base_addr->ARR = kArrVal;
    base_addr->EGR |= TIM_EGR_UG;
    base_addr->CR1 |= TIM_CR1_CEN;

    return true;
}

bool HwPwm::set_freq(uint32_t freq)
{
    // If not enabled
    if (!(base_addr->CR1 & TIM_CR1_CEN_Msk))
    {
        return false;
    }

    // If input not in range
    if (freq < 1 || pclk_freq < freq)
    {
        return false;
    }
    else if (max_edge_aligned_freq(pclk_freq) < freq &&
             settings.mode == PwmMode::EDGE_ALIGNED)
    {
        return false;
    }

    else if (max_center_aligned_freq(pclk_freq) < freq &&
             settings.mode != PwmMode::EDGE_ALIGNED)
    {
        return false;
    }

    // Compute according to the formula
    uint32_t psc_val = pclk_freq / (freq * (1 + kArrVal));

    if (settings.mode != PwmMode::EDGE_ALIGNED)
    {
        psc_val /= 2;
    }
    if (psc_val < 1 || kMaxPsc < psc_val)
    {
        return false;
    }

    // Save value
    base_addr->PSC = psc_val - 1;
    curr_freq = freq;

    return true;
}

bool HwPwm::set_duty_cycle(uint8_t duty_cycle)
{
    // If not enabled
    if (!(base_addr->CR1 & TIM_CR1_CEN_Msk))
    {
        return false;
    }
    // If not in range
    if (duty_cycle < 0 || 100 < duty_cycle)
    {
        return false;
    }

    // Compute according to the formula
    // duty_cycle = (CCR / ARR) * 100
    // duty_cycle / 100 = CCR / ARR
    // duty_cycle * ARR / 100 = CCR
    uint32_t ccr_val = static_cast<uint32_t>((kArrVal + 1) * duty_cycle / 100);

    switch (channel)
    {
        case PwmChannel::CH1:
            base_addr->CCR1 = ccr_val;
            break;

        case PwmChannel::CH2:
            base_addr->CCR2 = ccr_val;
            break;

        case PwmChannel::CH3:
            base_addr->CCR3 = ccr_val;
            break;

        case PwmChannel::CH4:
            base_addr->CCR4 = ccr_val;
            break;

        default:
            return false;
    }

    curr_duty_cycle = duty_cycle;

    return true;
}

}  // namespace Stmh7
}  // namespace LBR