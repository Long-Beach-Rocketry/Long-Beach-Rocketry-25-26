#include "delay.h"
#include <cstdint>

namespace
{
volatile uint32_t g_ms_ticks = 0;
}

// SysTick_Handler is defined in st_sys_clock.cc
// Provide a way for it to increment g_ms_ticks
extern "C" void IncDelayTicks(void)
{
    g_ms_ticks++;
}

namespace LBR::Utils
{

void DelayMs(uint32_t ms)
{
    uint32_t start;
    // Atomically read start time
    __asm__ volatile("cpsid i" : : : "memory");  // Disable interrupts
    start = g_ms_ticks;
    __asm__ volatile("cpsie i" : : : "memory");  // Enable interrupts

    while (1)
    {
        uint32_t now;
        // Atomically read current time
        __asm__ volatile("cpsid i" : : : "memory");
        now = g_ms_ticks;
        __asm__ volatile("cpsie i" : : : "memory");

        if ((now - start) >= ms)
            break;
        __asm__ volatile("nop");
    }
}

uint32_t GetMsTicks()
{
    uint32_t ticks;
    __asm__ volatile("cpsid i" : : : "memory");
    ticks = g_ms_ticks;
    __asm__ volatile("cpsie i" : : : "memory");
    return ticks;
}

}  // namespace LBR::Utils
