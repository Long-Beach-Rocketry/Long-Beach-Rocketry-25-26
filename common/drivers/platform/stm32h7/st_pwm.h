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

/**
 * @note Timers 12, 13, 14, 15, 16, 17 are only capable of edge-aligned mode
 * @note EDGE_ALIGNED: counter counts up or down depending on DIR bit; flag set when CNT == CCRx
 *       CENTER_ALIGNED_DOWN: counter counts up and down; flag set when counting down
 *       CENTER_ALIGNED_UP: counter counts up and down; flag set when counting up
 *       CENTER_ALIGNED_BOTH: counter counts up and down; flag set when counting up and down
 */
enum class PwmMode : uint8_t
{
    EDGE_ALIGNED = 0,
    CENTER_ALIGNED_DOWN,
    CENTER_ALIGNED_UP,
    CENTER_ALIGNED_UP_DOWN
};

/**
 * @note MODE1: Outputs HIGH while compare reg < count, else LOW
 *       MODE2: Ouputs LOW while compare reg < count, else HIGH
 */
enum class PwmOutputMode : uint8_t
{
    MODE1 = 6,
    MODE2 = 7
};

/**
 * @note Timers 12, 13, 14, 15, 16, 17 are not capable of downcounting
 * @note If in center-aligned mode, the direction cannot be controlled
 */
enum class PwmDir : uint8_t
{
    UPCOUNTING = 0,
    DOWNCOUNTING
};

/**
 * @note Timers 12, 15 can only use CH1 and CH2
 * @note Timers 13, 14, 16, 17 can only use CH1
 */
enum class PwmChannel : uint8_t
{
    CH1 = 1,
    CH2 = 2,
    CH3 = 3,
    CH4 = 4
};

/**
 * @brief Struct containing parameters to configure PWM
 */
struct StPwmSettings
{
    PwmMode mode;
    PwmOutputMode output_mode;
    PwmDir direction;
};

/**
 * @brief Collection of timer base address, pwm settings, timer channel, and peripheral clock frequency
 * @note Default peripheral clock frequency is 64 Mhz
 */
struct StPwmParams
{
    TIM_TypeDef* base_addr;
    PwmChannel channel;
    StPwmSettings settings;
    uint32_t pclk_freq = 64000000;
};

class HwPwm : public Pwm
{
public:
    /**
     * @brief Hw Contructor
     * @param params_ struct of timer and channel and peripheral clock frequency
     */
    explicit HwPwm(const StPwmParams& params_);

    /**
     * @brief Initializes PWM peripheral
     * 
     * @param None
     * @return true if successful, false otherwise
     */
    bool init();

    /**
     * @note PWM frequency equations:
     * Edge-aligned:   f_PWM = TimerClock / ((PSC + 1) * (ARR + 1))
     * Center-aligned: f_PWM = TimerClock / (2 * (PSC + 1) * (ARR + 1))
     * 
     * @note To find available frequencies, plug in PSC to the equations according to mode
     * Edge-aligned:   f_MAX = TimerClock / (PSC + 1)
     * Center-aligned: f_MAX = TimerClock / (2 * (PSC + 1))
     */

    /**
     * @brief Sets the pwm prescalar
     * @param freq value that will be computed to be the new prescalar
     * @return true if successful, false otherwise
     */
    bool set_freq(uint32_t freq) override;

    /**
     * @brief Sets the ccr value so the percent at which to start measuring
     * @param duty_cycle value that will be computed to be the new ccr value
     * @return true if successful, false otherwise
     */
    bool set_duty_cycle(uint8_t duty_cycle) override;

private:
    TIM_TypeDef* base_addr;
    PwmChannel channel;
    StPwmSettings settings;
    uint32_t curr_freq;
    uint8_t curr_duty_cycle;
    const uint32_t
        pclk_freq;  // const, since we probably don't want to change it ever
};

}  // namespace Stmh7
}  // namespace LBR