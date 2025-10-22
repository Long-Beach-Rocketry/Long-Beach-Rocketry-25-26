#include "delay.h"
#include "stm32l476xx.h"

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
    void DelayUs(uint32_t us) {
        for (volatile uint32_t i = 0; i < us * 4; ++i) {
            __NOP();
        }
    }
} // namespace LBR::Utils
       