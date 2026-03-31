/**
 * @file st_pwm.h
 * @brief PWM driver specfic for the stmh7.
 * @author Joseph Chang
 */

#pragma once
#include <cstdint>
#include "pwm.h"
#include "stm32h723xx.h"

namespace LBR
{
namespace Stmh7
{

enum class PwmMode : uint8_t
{
    EDGE_ALIGNED = 0,
    CENTER_ALIGNED_DOWN,
    CENTER_ALIGNED_UP,
    CENTER_ALIGNED_UP_DOWN
};

enum class PwmOutputMode : uint8_t
{
    MODE1 = 6,
    MODE2 = 7
};

enum class PwmDir : uint8_t
{
    UPCOUNTING = 0,
    DOWNCOUNTING
};

enum class PwmChannel : uint8_t
{
    CH1 = 1,
    CH2 = 2,
    CH3 = 3,
    CH4 = 4
};

struct StPwmSettings
{
    PwmMode mode;
    PwmOutputMode output_mode;
    PwmDir direction;
};

struct StPwmParams
{
    TIM_TypeDef* base_addr;
    PwmChannel channel;
    StPwmSettings settings;
};

class HwPwm : public Pwm
{
public:
    explicit HwPwm(const StPwmParams& params_);

    bool init();

    bool set_freq(uint32_t freq) override;

    bool set_duty_cycle(uint8_t duty_cycle) override;

private:
    TIM_TypeDef* base_addr;
    PwmChannel channel;
    StPwmSettings settings;
    uint32_t curr_freq;
    uint8_t curr_duty_cycle;
};

}  // namespace Stmh7
}  // namespace LBR