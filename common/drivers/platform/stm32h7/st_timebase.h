/**
* @file st_timebase.h
* @brief stm32H723 timebase driver interface
* @author Bex Saw
*/

#include <chrono>
#include <cstdint>
#include "stm32h7xx.h"
#include "timebase.h"

namespace LBR
{
namespace Stmh7
{

/**
* @brief The StTimebaseParams struct contains the parameters needed to initialize the hardware timebase.
* @param base_addr The base address of the timer peripheral (e.g., TIM2, TIM3, etc.)
* @param irq The IRQ number associated with the timer peripheral (e.g., TIM2_IRQn, TIM3_IRQn, etc.)
* @param timer_input_hz The input frequency of the timer in Hz. This is used to calculate the timer period for the desired tick period.
* @param enable_irq Whether to enable the timer interrupt. If true, the timer interrupt will be enabled and the overflow count will be incremented on each overflow.
*/
struct StTimebaseParams
{
    TIM_TypeDef* base_addr;
    IRQn_Type irq;
    uint32_t timer_input_hz;
    bool enable_irq;
};

class HwTimebase : public Timebase
{
public:
    explicit HwTimebase(const StTimebaseParams& params);

    /**
    * @brief init the timebase with a specified tick period
    * @param tick_period The period of the timebase ticks in microseconds
    */
    bool init(std::chrono::microseconds tick_period) override;

    /**
    * @brief start the timebase
    */
    void start() override;

    /**
    * @brief stop the timebase
    */
    void stop() override;

    /**
    * @brief get the current time in microseconds
    * @return The current time in microseconds
    */
    uint64_t now_us() const override;

    /**
    * @brief get the elapsed time in microseconds since a given time
    * @param since The starting time in microseconds
    * @return The elapsed time in microseconds
    */
    uint32_t elapsed_us(uint32_t since) const override;

    /**
    * @brief Set the Frequency of the TIM
    * @param new_timer_freq The desired new timer frequency
    * @param pclk The peripheral clock frequency in Hz
    * @return true if the timer frequency was set successfully, false otherwise
    */
    bool set_freq(uint32_t new_timer_freq, uint32_t pclk) override;

    /**
    * @brief Set the period of the desired timebase TIM channel
    * @param period The desired period in microseconds
    * @return true if the period was set successfully, false otherwise
    */
    bool set_period(std::chrono::microseconds period) override;

private:
    /**
    * @note TIM & IRQ for the timebase constructor init.
    */
    TIM_TypeDef* base_addr;
    IRQn_Type irq;

    /**
    * @timer_input_hz: The input frequency of the timer in Hz. 
    *                  This is used to calculate the timer period for the desired tick period.
    * @enable_irq: Whether to enable the timer interrupt. If true, the timer interrupt will be 
    *              enabled and the overflow count will be incremented on each overflow.
    * @overflow_count: A volatile counter that keeps track of the number of times the timer has overflowed.
    */
    uint32_t timer_input_hz;
    bool enable_irq;
    volatile uint64_t overflow_count;
};
}  // namespace Stmh7
}  // namespace LBR