/**
 * @file main.cc
 * @brief Timebase driver test app
 */

#include "board.h"

#include <cstdio>
#include <span>

using namespace LBR;

static constexpr uint32_t kPrintIntervalUs{100'000};  // 100 ms

// This test is printing out the uptime_us every 100 ms
// The elapsed_time is also printed to verify that the timebase is working correctly
// The timebase is implemented using TIM2, which is configured to generate an interrupt every 1 ms

int main()
{
    bsp_init();
    Board board = get_board();

    char message[96];
    uint64_t previous_us = 0;

    while (1)
    {
        const uint64_t uptime_us = board.timebase.uptime_us();

        // Print every 100 ms.
        if ((uptime_us - previous_us) >= kPrintIntervalUs)
        {
            // Update previous_us and calculate elapsed time
            const uint64_t elapsed_us = uptime_us - previous_us;
            previous_us = uptime_us;

            const int length = std::snprintf(
                message, sizeof(message), "uptime_us=%llu elapsed_us=%llu\r\n",
                static_cast<unsigned long long>(uptime_us),
                static_cast<unsigned long long>(elapsed_us));

            if (length > 0)
            {
                board.usart.send(std::span<const uint8_t>{
                    reinterpret_cast<const uint8_t*>(message),
                    static_cast<std::size_t>(length)});
            }
        }
    }
}