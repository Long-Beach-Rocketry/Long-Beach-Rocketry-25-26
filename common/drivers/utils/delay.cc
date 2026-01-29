#include "delay.h"
#include <cstdint>

namespace {
volatile uint32_t g_ms_ticks = 0;
}

extern "C" void SysTick_Handler(void) {
    g_ms_ticks++;
}

namespace LBR::Utils {

void DelayMs(uint32_t ms) {
    uint32_t start = g_ms_ticks;
    while ((g_ms_ticks - start) < ms) {
        __NOP();
    }
}

uint32_t GetMsTicks() {
    return g_ms_ticks;
}

} // namespace LBR::Utils
