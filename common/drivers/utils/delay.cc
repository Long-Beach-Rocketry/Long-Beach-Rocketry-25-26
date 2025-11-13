#include "delay.h"

#ifdef STM32L476xx
#include "stm32l476xx.h"
#endif

namespace LBR::Utils {
    void DelayMs(uint32_t ms) {
        
        for (volatile uint32_t i = 0; i < ms * 4000; ++i) {
            __NOP();
        }

        void DelayUs(uint32_t us) {
        for (volatile uint32_t i = 0; i < us * 4; ++i) {
            __NOP();
        }
    }
#else
    // Native/host target: stub or use std sleep if needed
    (void)ms;
#endif
}
}; // namespace LBR::Utils
       