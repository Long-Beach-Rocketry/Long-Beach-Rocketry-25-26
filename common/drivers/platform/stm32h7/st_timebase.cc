#include "st_timebase.h"

namespace LBR
{
namespace Stmh7
{

static constexpr uint32_t kMaxARR32{0xFFFFFFFF};
static constexpr uint32_t kMaxARR16{0xFFFF};
static constexpr uint32_t kMicrosecondsPerSecond{1'000'000};

HwTimebase::HwTimebase(const StTimebaseParams& params)
    : base_addr{params.base_addr},
      irq{params.irq},
      timer_input_hz{params.timer_input_hz},
      enable_irq{params.enable_irq}
{
}

static inline bool is_supported_timer(TIM_TypeDef* tim)
{
    return tim == TIM1 || tim == TIM2 || tim == TIM3 || tim == TIM4 ||
           tim == TIM5 || tim == TIM6 || tim == TIM7 || tim == TIM8 ||
           tim == TIM12 || tim == TIM13 || tim == TIM14 || tim == TIM15 ||
           tim == TIM16 || tim == TIM17 || tim == TIM23 || tim == TIM24;
}

static inline bool is_32bit_timer(TIM_TypeDef* tim)
{
    return tim == TIM2 || tim == TIM5 || tim == TIM23 || tim == TIM24;
}

bool HwTimebase::init(std::chrono::microseconds tick_period)
{
    // Check if base_addr is valid (not nullptr)
    if (base_addr == nullptr)
    {
        return false;
    }

    // Check if tick_period is valid (greater than zero)
    if (tick_period.count() <= 0)
    {
        return false;
    }

    // timer_input_hz must be nonzero or prescaler math will divide by zero.
    if (timer_input_hz == 0)
    {
        return false;
    }

    // ONLY TIM1 to TIM17 + TIM23 & TIM24 if out of scope then reject or unsupported
    if (!is_supported_timer(base_addr))
    {
        return false;
    }

    // Calculate the maximum ARR value based on whether the timer is 16-bit or 32-bit.
    const uint32_t max_arr = is_32bit_timer(base_addr) ? kMaxARR32 : kMaxARR16;

    // Stop timer before configuring it by clearing the CEN bit in CR1.
    base_addr->CR1 &= ~TIM_CR1_CEN;

    // configure timer mode (upcounting, edge-aligned, etc.) and enable auto-reload preload if desired.

    // Use internal clock
    base_addr->SMCR &= ~TIM_SMCR_SMS;  // SMS = 0: Internal clock mode

    // Upcounting mode
    base_addr->CR1 &= ~TIM_CR1_DIR;  // DIR = 0: Upcounting mode

    // Edge-aligned mode.
    base_addr->CR1 &= ~TIM_CR1_CMS;  // CMS = 0: Edge-aligned mode

    // Enable auto-reload preload.
    base_addr->CR1 |= TIM_CR1_ARPE;  // ARPE = 1: Auto-reload preload enabled

    // Only overflow/underflow generates update interrupt/request.
    base_addr->CR1 |=
        TIM_CR1_URS;  // URS = 1: Only overflow/underflow generates update interrupt/request

    // Convert tick period to raw timer ticks
    required_ticks = static_cast<uint64_t>(
        (tick_period.count() * static_cast<uint64_t>(timer_input_hz)) /
        kMicrosecondsPerSecond);

    // Check if required_ticks is valid (greater than zero)
    if (required_ticks == 0)
    {
        return false;
    }

    // Pick prescaler so ARR fits
    uint32_t prescaler = 0;
    uint64_t arr_ticks = required_ticks;

    while (arr_ticks > max_arr)
    {
        prescaler++;

        if (prescaler > kMaxARR16)
        {
            return false;
        }

        arr_ticks = required_ticks / (prescaler + 1);

        if (arr_ticks == 0)
        {
            return false;
        }
    }

    // set prescaler and auto-reload register
    base_addr->PSC = prescaler;
    base_addr->ARR = static_cast<uint32_t>(arr_ticks - 1);

    // Reset CNT to 0
    base_addr->CNT = 0;

    // Clear UIF flag in SR
    base_addr->SR &= ~TIM_SR_UIF;

    // IRQ specific handler
    if (enable_irq)
    {
        // DIER is for enabling/disabling interrupts. UIE is the update interrupt enable bit.
        base_addr->DIER |= TIM_DIER_UIE;

        // Set the priority of the interrupt and enable it in the NVIC if enable_irq is true.
        NVIC_SetPriority(irq, 0);
        NVIC_EnableIRQ(irq);
    }
    else
    {
        // Disable the update interrupt if enable_irq is false.
        base_addr->DIER &= ~TIM_DIER_UIE;
    }

    // Load PSC/ARR immediately
    base_addr->EGR |= TIM_EGR_UG;

    // Clear UIF again because UG may set it
    base_addr->SR &= ~TIM_SR_UIF;

    return true;
}

bool HwTimebase::set_freq(uint32_t new_timer_freq, uint32_t pclk)
{
    // Check if new_timer_freq is in valid range

    // Before feeding in the new desired resolution that we want

    // we need to have a condition check if the current TIM is running

    // if it was running we need to stop() it and then reconfigure the TIM with the new desired resolution

    // once stop() we can go ahead and feed new configuration and then start() it again

    // Set the achievable counter clock after integer division

    return true;
}

bool HwTimebase::set_period(std::chrono::microseconds period)
{
    // Implementation for setting the period of the timebase
    return true;
}

void HwTimebase::start()
{
    base_addr->CR1 |= TIM_CR1_CEN;
}

void HwTimebase::stop()
{
    base_addr->CR1 &= ~TIM_CR1_CEN;
}

uint64_t HwTimebase::now_us() const
{
    return 0;
}

uint32_t HwTimebase::elapsed_us(uint32_t since) const
{
    // Implementation for getting the elapsed time in microseconds since a given time
    return 0;
}
}  // namespace Stmh7
}  // namespace LBR
