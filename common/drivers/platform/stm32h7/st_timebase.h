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
    */
    bool init() override;

    /**
    * @brief start the timebase
    */
    bool start() override;

    /**
    * @brief stop the timebase
    */
    void stop() override;

    /**
    * @brief get the current time in microseconds
    * @return The current time in microseconds
    */
    uint64_t uptime_us() const override;

    /**
    * @brief get the raw elapsed counter ticks (not converted to microseconds)
    * @return Total elapsed counter ticks since the timebase started.
    */
    uint64_t uptime_ticks() const override;

    /**
    * @brief Calculate the elapsed time since a previously captured uptime value.
    *
    * @param since_us A timestamp previously returned by uptime_us().
    * @return The elapsed duration in microseconds.
    */
    uint64_t elapsed_since_us(uint64_t since_us) const override;

    /**
    * @brief Set the counter (tick) frequency of the TIM
    * @param new_counter_freq The desired counter frequency in Hz. Must divide
    * the fixed timer input clock (timer_input_hz) exactly.
    * @return true if the counter frequency was set successfully, false otherwise
    */
    bool set_freq(uint32_t new_counter_freq) override;

    /**
    * @brief Set the period of the desired timebase TIM channel
    * @param period The desired period in microseconds
    * @return true if the period was set successfully, false otherwise
    */
    bool set_period(std::chrono::microseconds period) override;

    /**
    * @brief Handle the timer interrupt. This function should be called from the timer's IRQ handler.
    */
    void handle_irq();

private:
    TIM_TypeDef* base_addr;
    IRQn_Type irq;
    uint32_t timer_input_hz;
    bool enable_irq;

    uint64_t required_ticks{0};
    bool configured{false};
    volatile uint32_t overflow_count{0};
};
}  // namespace Stmh7
}  // namespace LBR