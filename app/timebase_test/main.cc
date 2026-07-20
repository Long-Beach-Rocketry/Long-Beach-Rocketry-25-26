/**
 * @file main.cc
 * @brief Timebase driver test app (LED blink)
 */

#include "board.h"
#include "delay.h"

#include <cstdint>

using namespace LBR;

/*
 * @note: Dynamic-frequency blink test for the timebase driver.
 *
 * The LED toggles every kBlink *raw counter ticks* (uptime_ticks), NOT
 * microseconds. Ticks accrue faster at a higher counter frequency, so the blink
 * rate scales with the timebase frequency the loop cycles through below.
 *
 * With kBlink = 50'000 ticks:
 *   counter freq   50'000 ticks   half period   blink
 *      1 MHz         50'000 us       50 ms       10 Hz
 *    500 kHz        100'000 us      100 ms        5 Hz
 *      2 MHz         25'000 us       25 ms       20 Hz
 *
 * (Blinking on uptime_us() instead would be frequency INDEPENDENT - real
 *  microseconds - so the rate would not change. This test uses ticks on purpose
 *  to make the frequency change visible.)
 */
static constexpr uint64_t kBlink{50'000};

static constexpr uint64_t kOneMHz{1'000'000};
static constexpr uint64_t kFiveHundredkHz{500'000};
static constexpr uint64_t kTwoMHz{2'000'000};

int main(int argc, char* argv[])
{
    bsp_init();
    Board board = get_board();

    board.timebase.start();
    board.timebase.set_freq(kOneMHz);

    uint64_t last_toggle_ticks = board.timebase.uptime_ticks();
    uint64_t last_freq_change_us = board.timebase.uptime_us();
    uint32_t frequency_hz = kOneMHz;

    while (1)
    {
        const uint64_t current_ticks = board.timebase.uptime_ticks();

        if ((current_ticks - last_toggle_ticks) >= kBlink)
        {
            board.led.toggle();
            last_toggle_ticks = current_ticks;
        }

        // Change counter frequency every 5 real seconds.
        if (board.timebase.elapsed_since_us(last_freq_change_us) >= 5'000'000)
        {
            if (frequency_hz == kOneMHz)
            {
                frequency_hz = kFiveHundredkHz;
            }
            else if (frequency_hz == kFiveHundredkHz)
            {
                frequency_hz = kTwoMHz;
            }
            else
            {
                frequency_hz = kOneMHz;
            }

            board.timebase.set_freq(frequency_hz);
            last_freq_change_us = board.timebase.uptime_us();

            // Reset after reconfiguring to avoid a false immediate toggle.
            last_toggle_ticks = board.timebase.uptime_ticks();
        }
    }

    // Printing every 100ms test example
    // const uint64_t uptime_us = board.timebase.uptime_us();
    //
    // // Print every 100 ms.
    // if ((uptime_us - previous_us) >= kPrintIntervalUs)
    // {
    //     // Update previous_us and calculate elapsed time
    //     const uint64_t elapsed_us = uptime_us - previous_us;
    //     previous_us = uptime_us;
    //
    //     const int length = std::snprintf(
    //         message, sizeof(message), "uptime_us=%llu elapsed_us=%llu\r\n",
    //         static_cast<unsigned long long>(uptime_us),
    //         static_cast<unsigned long long>(elapsed_us));
    //
    //     if (length > 0)
    //     {
    //         board.usart.send(std::span<const uint8_t>{
    //             reinterpret_cast<const uint8_t*>(message),
    //             static_cast<std::size_t>(length)});
    //     }
}
