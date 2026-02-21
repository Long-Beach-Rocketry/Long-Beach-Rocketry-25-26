<<<<<<< HEAD
#include "delay.h"
#include <cstdint>

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

}  // namespace LBR::Utils
=======
#include "delay.h"

#ifdef STM32L476xx
#include "stm32l476xx.h"
#endif

namespace LBR::Utils
// NOTE: This delay implementation is temporary.
// It is hardware-specific and may become inaccurate if the clock speed changes.
{

void DelayMs(uint32_t ms)
{
#ifdef STM32L476xx
    // Embedded target: busy-wait loop
    for (uint32_t i = 0; i < ms * 4000; i++)
    {
        __NOP();
    }
#else
    // Native/host target: stub or use std sleep if needed
    (void)ms;
#endif
}

void DelayUs(uint32_t us)
{
#ifdef STM32L476xx
    for (uint32_t i = 0; i < us * 4; i++)
    {
        __NOP();
    }
#else
    (void)us;
#endif
}

}  // namespace LBR::Utils
>>>>>>> 319d772 (ekf)
