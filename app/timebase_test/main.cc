/**
 * @file main.cc
 * @brief Timebase driver test: report ISR-driven overflows over UART.
 */

#include "board.h"

#include <cstdint>
#include <cstdio>
#include <span>

using namespace LBR;

/*
 * @note Each counter overflow fires the timer update interrupt, whose ISR
 * (HwTimebase::handle_irq) increments the driver's private overflow_count. We
 * recover that same count here as uptime_ticks() / kPeriodTicks.
 *
 * TIM3 is 16-bit (ARR = 0xFFFF), so one overflow is 0xFFFF + 1 = 65536 ticks.
 * With the counter pinned at 1 MHz (1 tick = 1 us), every overflow should take
 * ~65536 us (~65.5 ms). A steady per-overflow time means the ISR is firing at
 * the right rate; drift or jitter points at a bug.
 */
static constexpr uint64_t kPeriodTicks{
    65'536};  // TIM3 (16-bit): ARR (0xFFFF) + 1
char message[96];

int main(int argc, char* argv[])
{
    bsp_init();
    Board board = get_board();

    board.timebase.start();
    board.timebase.set_freq(1'000'000);

    uint64_t last_overflow = board.timebase.uptime_ticks() / kPeriodTicks;
    uint64_t last_time_us = board.timebase.uptime_us();

    while (1)
    {
        const uint64_t overflows = board.timebase.uptime_ticks() / kPeriodTicks;

        // No new overflow yet: keep polling.
        if (overflows == last_overflow)
        {
            continue;
        }

        const uint64_t now_us = board.timebase.uptime_us();

        // Print 32-bit values with %lu: this target's newlib-nano snprintf has
        // 64-bit (%llu) support compiled out, so %llu would render as literal
        // "lu". The count and per-overflow time both fit comfortably in 32 bits.
        const int length =
            std::snprintf(message, sizeof(message),
                          "overflow #%lu  took=%lu us  (expected ~%lu)\r\n",
                          static_cast<unsigned long>(overflows),
                          static_cast<unsigned long>(now_us - last_time_us),
                          static_cast<unsigned long>(kPeriodTicks));

        if (length > 0)
        {
            board.usart.send(std::span<const uint8_t>{
                reinterpret_cast<const uint8_t*>(message),
                static_cast<std::size_t>(length)});
        }

        last_overflow = overflows;
        last_time_us = now_us;
    }
}
