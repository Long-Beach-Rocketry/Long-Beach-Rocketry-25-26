/**
 * @file main_transmit.cc
 * @author Alex Pulido
 * @brief Continuously transmit test packets from an SX1262.
 */

#include <span>
#include <cstddef>
#include <cstdint>

#include "board.h"
#include "delay.h"
#include "test_config.h"

using namespace LBR;

volatile uint32_t transmit_successes = 0;
volatile uint32_t transmit_failures = 0;

int main()
{
    bsp_init();

    Board& board = get_board();

    // Payload message is defined in test_config.h as kSx1262TestPayload
    Sx1262Params radio_params{
        .settings = sx1262_test_settings(),
        .spi = board.spi,
        .cs = board.cs,
        .reset = board.reset,
        .busy = board.busy,
    };
    Sx1262 radio{radio_params};

    radio.init();

    while (1)
    {
        const std::span<const uint8_t> payload{kSx1262TestPayload};
        if (radio.transmit(payload))
        {
            ++transmit_successes;
        }
        else
        {
            ++transmit_failures;
        }
        Utils::DelayMs(500);
    }
}