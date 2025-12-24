/**
 * @file st_pwm.h
 * @brief PWM driver for STM32L476xx
 * @author Yshi Blanco
 * @date 12/24/2025
 */

#include <cstdint>
#include "pwm.h"
#include "stm32l476xx.h"

namespace LBR
{
namespace Stml4
{

/**
 * @brief Collection of timer base address and timer channel
 */
struct StPwmParams
{
    TIM_TypeDef* base_addr;
    uint8_t channel;
};

class HwPwm : public Pwm
{
public:
    /**
     * @brief Hw Contructor
     * @param params struct of timer and channel
     */
    explicit HwPwm(const StPwmParams& params);

    /**
     * @brief Initializes PWM peripheral
     * 
     * @param None
     * @return true if successful, false otherwise
     */
    bool init();

    bool set_freq(uint32_t freq) override;
    bool set_duty_cycle(float duty_cycle) override;

private:
    TIM_TypeDef* _base_addr;
    uint8_t _channel;

    /**
     * @brief Gets clock frequency of the AHB bus
     * @return Clock frequency in Hz
     */
    uint32_t get_hclk_freq();

    /**
     * @brief Gets clock frequency of the APB1 bus
     * @return Clock frequency in Hz
     * 
     * @note used by timers 2-7
     */
    uint32_t get_pclk1_freq();

    /**
     * @brief Gets clock frequency of the APB2 bus
     * @return Clock frequency in Hz
     * 
     * @note used by timers 1, 8, 15, 16, 17
     */
    uint32_t get_pclk2_freq();
};

}  // namespace Stml4
}  // namespace LBR