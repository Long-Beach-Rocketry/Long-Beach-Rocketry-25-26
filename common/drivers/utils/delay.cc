#include "delay.h"
#include <cstdint>

// This is because the DWT cycle counter is only available on ARM Cortex-M cores,
// and the SysTick timer is also ARM-specific. For non-ARM platforms, we provide
// no-op implementations of these functions.
#if defined(__arm__)
#if defined(STM32H723xx)
#include "stm32h7xx.h"
#elif defined(STM32L476xx)
#include "stm32l4xx.h"
#endif
#endif

namespace
{
volatile uint32_t g_ms_ticks = 0;

// ARM-specific interrupt disable/enable macros
#ifdef __arm__
#define DISABLE_IRQ() __asm__ volatile("cpsid i" : : : "memory")
#define ENABLE_IRQ() __asm__ volatile("cpsie i" : : : "memory")
#else
// For non-ARM (native builds), these are no-ops
#define DISABLE_IRQ()
#define ENABLE_IRQ()
#endif
}  // namespace

// SysTick_Handler is defined in st_sys_clock.cc
// Provide a way for it to increment g_ms_ticks
extern "C" void IncDelayTicks(void)
{
    g_ms_ticks = g_ms_ticks + 1;
}

namespace LBR::Utils
{

void DelayMs(uint32_t ms)
{
    uint32_t start;
    // Atomically read start time
    DISABLE_IRQ();
    start = g_ms_ticks;
    ENABLE_IRQ();

    while (1)
    {
        uint32_t now;
        // Atomically read current time
        DISABLE_IRQ();
        now = g_ms_ticks;
        ENABLE_IRQ();

        if ((now - start) >= ms)
            break;
#ifdef __arm__
        __asm__ volatile("nop");
#endif
    }
}

uint32_t GetMsTicks()
{
    uint32_t ticks;
    DISABLE_IRQ();
    ticks = g_ms_ticks;
    ENABLE_IRQ();
    return ticks;
}

#if defined(STM32H723xx) || defined(STM32L476xx)

void EnableUsTimer()
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

uint32_t GetUs()
{
    // DWT->CYCCNT wraps at ~67s @ 64 MHz — fine for loopback timing
    return DWT->CYCCNT / (SystemCoreClock / 1000000UL);
}

#else

void EnableUsTimer()
{
}

uint32_t GetUs()
{
    return 0;
}

#endif

}  // namespace LBR::Utils
