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
