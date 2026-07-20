#include "st_timebase.h"

namespace LBR
{
namespace Stmh7
{

static constexpr uint32_t kMaxPsc{0xFFFFU};
static constexpr uint32_t kMaxArr16{0xFFFFU};
static constexpr uint32_t kMaxArr32{0xFFFFFFFFU};
static constexpr uint64_t kMaxArr64{0xFFFFFFFFFFFFFFFFULL};
static constexpr uint32_t kMicrosecondsPerSecond{1'000'000U};
static constexpr uint32_t kDefaultCounterHz{1'000'000U};

/******************************************************************************
 * STM32H723 Timers
 *
 * TIM1  - Advanced Timer,        16-bit, 4 Channels
 * TIM2  - General-Purpose Timer, 32-bit, 4 Channels
 * TIM3  - General-Purpose Timer, 16-bit, 4 Channels
 * TIM4  - General-Purpose Timer, 16-bit, 4 Channels
 * TIM5  - General-Purpose Timer, 32-bit, 4 Channels
 * TIM6  - Basic Timer,           16-bit
 * TIM7  - Basic Timer,           16-bit
 * TIM8  - Advanced Timer,        16-bit, 4 Channels
 * TIM12 - General-Purpose Timer, 16-bit, 2 Channels
 * TIM13 - General-Purpose Timer, 16-bit, 1 Channel
 * TIM14 - General-Purpose Timer, 16-bit, 1 Channel
 * TIM15 - General-Purpose Timer, 16-bit, 2 Channels
 * TIM16 - General-Purpose Timer, 16-bit, 1 Channel
 * TIM17 - General-Purpose Timer, 16-bit, 1 Channel
 * TIM23 - General-Purpose Timer, 32-bit, 4 Channels (if available)
 * TIM24 - General-Purpose Timer, 32-bit, 4 Channels (if available)
 ******************************************************************************/

static inline bool is_supported_timer(const TIM_TypeDef* tim)
{
    return tim == TIM1 || tim == TIM2 || tim == TIM3 || tim == TIM4 ||
           tim == TIM5 || tim == TIM6 || tim == TIM7 || tim == TIM8 ||
           tim == TIM12 || tim == TIM13 || tim == TIM14 || tim == TIM15 ||
           tim == TIM16 || tim == TIM17 || tim == TIM23 || tim == TIM24;
}

static inline bool is_32bit_timer(const TIM_TypeDef* tim)
{
    return tim == TIM2 || tim == TIM5 || tim == TIM23 || tim == TIM24;
}

static inline uint32_t max_arr(const TIM_TypeDef* tim)
{
    return is_32bit_timer(tim) ? kMaxArr32 : kMaxArr16;
}

HwTimebase::HwTimebase(const StTimebaseParams& params)
    : base_addr{params.base_addr},
      irq{params.irq},
      timer_input_hz{params.timer_input_hz},
      enable_irq{params.enable_irq}
{
}

bool HwTimebase::init()
{
    configured = false;
    required_ticks = 0U;
    overflow_count = 0U;

    // Validate the parameters
    if (base_addr == nullptr || timer_input_hz == 0U ||
        !is_supported_timer(base_addr))
    {
        return false;
    }

    // If interrupts are not enabled, we cannot use the timebase
    if (!enable_irq)
    {
        return false;
    }

    // Require an exact integer divider to achieve the desired counter frequency
    if ((timer_input_hz % kDefaultCounterHz) != 0U)
    {
        return false;
    }

    // Calculate the divider to achieve a 1 MHz counter frequency (1 tick = 1 microsecond)
    const uint32_t divider = timer_input_hz / kDefaultCounterHz;

    // Check if the divider is within the valid range for the timer's prescaler
    if (divider == 0U || divider > (kMaxPsc + 1U))
    {
        return false;
    }

    // Disable the timer and its interrupt before configuration
    base_addr->CR1 &= ~TIM_CR1_CEN;
    base_addr->DIER &= ~TIM_DIER_UIE;

    // Configure the timer in upcounting mode with no clock division
    base_addr->SMCR &= ~TIM_SMCR_SMS;
    base_addr->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);

    // Enable auto-reload preload and update request source
    base_addr->CR1 |= TIM_CR1_ARPE;
    base_addr->CR1 |= TIM_CR1_URS;

    // Set the prescaler and auto-reload register to achieve the desired counter frequency
    base_addr->PSC = static_cast<uint16_t>(divider - 1U);
    base_addr->ARR = max_arr(base_addr);
    base_addr->CNT = 0U;

    required_ticks = static_cast<uint64_t>(base_addr->ARR) + 1U;

    // Feed the update event to load the prescaler and auto-reload values into the timer
    base_addr->EGR = TIM_EGR_UG;
    base_addr->SR &= ~TIM_SR_UIF;

    base_addr->DIER |= TIM_DIER_UIE;

    configured = true;
    return true;
}

bool HwTimebase::start()
{
    // Check if the timer is configured and the base address is valid
    if (base_addr == nullptr || !configured)
    {
        return false;
    }

    // Ensure the timer is paused before (re)starting so we don't corrupt an
    // already-running timebase
    if ((base_addr->CR1 & TIM_CR1_CEN) != 0U)
    {
        return false;
    }

    // Enable the timer by setting the CEN bit in the CR1 register
    base_addr->CR1 |= TIM_CR1_CEN;
    return true;
}

void HwTimebase::stop()
{
    // Check if the timer is configured and the base address is valid
    if (base_addr != nullptr && configured)
    {
        // Disable the timer by clearing the CEN bit in the CR1 register
        base_addr->CR1 &= ~TIM_CR1_CEN;
    }
}

bool HwTimebase::set_freq(uint32_t new_counter_freq)
{
    // Check if the timer is configured and the base address is valid
    if (base_addr == nullptr || !configured)
    {
        return false;
    }

    // The timer input clock is fixed at construction (timer_input_hz), so the
    // requested counter frequency is validated directly against it.
    if (new_counter_freq == 0U || new_counter_freq > timer_input_hz)
    {
        return false;
    }

    // Ensure that the new counter frequency is an exact integer divisor of the timer clock
    if ((timer_input_hz % new_counter_freq) != 0U)
    {
        return false;
    }

    // Calculate the divider to achieve the desired counter frequency
    const uint32_t divider = timer_input_hz / new_counter_freq;

    // Check if the divider is within the valid range for the timer's prescaler
    if (divider == 0U || divider > (kMaxPsc + 1U))
    {
        return false;
    }

    // Save the current running state of the timer
    const bool was_running = (base_addr->CR1 & TIM_CR1_CEN) != 0U;

    // Disable the timer before reconfiguration
    stop();

    // Set the new prescaler value to achieve the desired counter frequency
    base_addr->PSC = static_cast<uint16_t>(divider - 1U);
    base_addr->CNT = 0U;
    overflow_count = 0U;

    // Feed the update event to load the new prescaler value into the timer
    base_addr->EGR = TIM_EGR_UG;
    base_addr->SR &= ~TIM_SR_UIF;
    NVIC_ClearPendingIRQ(irq);

    // Restore the timer's running state if it was previously running
    if (was_running)
    {
        base_addr->CR1 |= TIM_CR1_CEN;
    }

    return true;
}

bool HwTimebase::set_period(std::chrono::microseconds period)
{
    // Check if the timer is configured and the base address is valid
    if (base_addr == nullptr || !configured)
    {
        return false;
    }

    // Validate the requested period to ensure it is positive
    if (period.count() <= 0)
    {
        return false;
    }

    // Calculate the effective counter frequency based on the timer's prescaler
    const uint32_t divider = base_addr->PSC + 1U;
    const uint32_t counter_hz = timer_input_hz / divider;

    // Validate the counter frequency to ensure it is non-zero
    if (counter_hz == 0U)
    {
        return false;
    }

    // Convert the requested period from microseconds to ticks based on the counter frequency
    const uint64_t period_us = static_cast<uint64_t>(period.count());

    // Check for potential overflow when calculating the number of ticks for the requested period
    if (period_us > (kMaxArr64 / static_cast<uint64_t>(counter_hz)))
    {
        return false;
    }

    // Calculate the number of ticks required for the requested period, rounding to the nearest tick
    uint64_t period_ticks =
        ((period_us * counter_hz) + (kMicrosecondsPerSecond / 2U)) /
        kMicrosecondsPerSecond;

    // Ensure that the period in ticks is at least 1 to avoid setting an invalid period
    if (period_ticks == 0U)
    {
        period_ticks = 1U;
    }

    // Check if the requested period exceeds the maximum allowable period for the timer
    const uint64_t max_period_ticks =
        static_cast<uint64_t>(max_arr(base_addr)) + 1U;

    // If the requested period exceeds the maximum allowable period, return false to indicate failure
    if (period_ticks > max_period_ticks)
    {
        return false;
    }

    // Save the current running state of the timer before reconfiguration
    const bool was_running = (base_addr->CR1 & TIM_CR1_CEN) != 0U;

    // Disable the timer before reconfiguration
    stop();

    // Set the auto-reload register (ARR) to the calculated period in ticks, subtracting 1 since ARR is zero-based
    base_addr->ARR = static_cast<uint32_t>(period_ticks - 1U);

    // Reset the counter and overflow count to start fresh with the new period
    base_addr->CNT = 0U;
    overflow_count = 0U;
    required_ticks = period_ticks;
    base_addr->EGR = TIM_EGR_UG;
    base_addr->SR &= ~TIM_SR_UIF;

    // Clear any pending interrupts in the NVIC to avoid false triggers after reconfiguration
    NVIC_ClearPendingIRQ(irq);

    // Restore the timer's running state if it was previously running before reconfiguration
    if (was_running)
    {
        base_addr->CR1 |= TIM_CR1_CEN;
    }

    return true;
}

uint64_t HwTimebase::uptime_ticks() const
{
    if (base_addr == nullptr || !configured)
    {
        return 0U;
    }

    /*
     * overflow_count is uint32_t so a read is a single atomic access on
     * Cortex-M7 (a 64-bit read would tear and defeat this snapshot).
     *
     * Reading CNT does not modify overflow_count; only handle_irq() does.
     * However the overflow ISR can fire *between* our reads, pairing a fresh
     * overflow_count with a stale CNT. Re-snapshot until the two overflow
     * reads agree so counter is guaranteed to belong to that interval.
     */
    uint32_t overflow_before;
    uint32_t overflow_after;
    uint32_t counter;

    // Loop until a consistent snapshot of the overflow count and counter is obtained
    do
    {
        overflow_before = overflow_count;
        counter = base_addr->CNT;
        overflow_after = overflow_count;
    } while (overflow_before != overflow_after);

    /*
     * The snapshot above is only race-free against overflows the ISR has
     * already serviced. There is still a window where the counter has just
     * wrapped in hardware (UIF set) but handle_irq() has not yet run to bump
     * overflow_count. If we see UIF set, account for that pending overflow
     * ourselves (+1) and re-read CNT so counter reflects the post-wrap value
     * that belongs to the incremented overflow interval. Without this, uptime
     * would briefly jump backwards each time the timer rolls over.
     */
    if ((base_addr->SR & TIM_SR_UIF) != 0U)
    {
        overflow_after += 1U;
        counter = base_addr->CNT;
    }

    const uint64_t period_ticks = static_cast<uint64_t>(base_addr->ARR) + 1U;

    // Raw tick count. This scales with the counter frequency: doubling the
    // counter frequency doubles how fast this grows.
    return static_cast<uint64_t>(overflow_after) * period_ticks +
           static_cast<uint64_t>(counter);
}

uint64_t HwTimebase::uptime_us() const
{
    // Take the raw tick snapshot (handles the null/unconfigured case too).
    const uint64_t total_ticks = uptime_ticks();

    if (base_addr == nullptr || !configured)
    {
        return 0U;
    }

    const uint64_t counter_hz =
        timer_input_hz / (static_cast<uint64_t>(base_addr->PSC) + 1U);

    // If the counter frequency is equal to 1 MHz, return the total ticks directly as microseconds
    if (counter_hz == kMicrosecondsPerSecond)
    {
        return total_ticks;
    }

    const uint64_t whole_seconds = total_ticks / counter_hz;
    const uint64_t remaining_ticks = total_ticks % counter_hz;

    // Calculate the total uptime in microseconds by combining whole seconds and remaining ticks converted to microseconds
    return (whole_seconds * kMicrosecondsPerSecond) +
           ((remaining_ticks * kMicrosecondsPerSecond) / counter_hz);
}

uint64_t HwTimebase::elapsed_since_us(uint64_t since_us) const
{
    const uint64_t now_us = uptime_us();

    // If the current uptime is less than the provided timestamp
    if (now_us < since_us)
    {
        return 0U;
    }

    // Calculate the differences
    return now_us - since_us;
}

void HwTimebase::handle_irq()
{
    if (base_addr == nullptr)
    {
        return;
    }

    // Check if the update interrupt flag (UIF) is set and the update interrupt is enabled (UIE)
    if (((base_addr->SR & TIM_SR_UIF) != 0U) &&
        ((base_addr->DIER & TIM_DIER_UIE) != 0U))
    {
        base_addr->SR &= ~TIM_SR_UIF;
        ++overflow_count;
    }
}

}  // namespace Stmh7
}  // namespace LBR