/**
 * @file st_pwm.h
 * @brief PWM driver for STM32L476xx
 * @author Yshi Blanco
 * @date 12/24/2025
 */

#include "pwm.h"
#include "stm32l476xx.h"

namespace LBR
{
namespace Stml4
{

/**
 * @note Timers 15, 16, 17 are only capable of edge-aligned mode
 * @note CENTER_ALIGNED_DOWN: flag when counting down
 *       CENTER_ALIGNED_UP: flag set when counting up
 *       CENTER_ALIGNED_BOTH: flag set when counting up and down
 */
enum class PwmMode : uint8_t
{
    EDGE_ALIGNED = 0,
    CENTER_ALIGNED_DOWN,
    CENTER_ALIGNED_UP,
    CENTER_ALIGNED_BOTH
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
 * @note Timers 15, 16, 17 are not capable of downcounting
 * @note If in center-aligned mode, the direction cannot be controlled
 */
enum class PwmDir : uint8_t
{
    UPCOUNTING = 0,
    DOWNCOUNTING = TIM_CR1_DIR
};

/**
 * @brief Struct containing parameters to configure PWM
 */
struct StPwmSettings
{
    PwmMode mode;
    PwmOutputMode output_mode;
    PwmDir dir;
};

/**
 * @brief Collection of timer base address and timer channel
 */
struct StPwmParams
{
    TIM_TypeDef* base_addr;
    uint8_t channel;
    StPwmSettings settings;
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
    StPwmSettings _settings;
    uint32_t _curr_freq;
    float _curr_duty_cycle;
};

}  // namespace Stml4
}  // namespace LBR