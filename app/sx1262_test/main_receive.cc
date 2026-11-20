/**
 * @file main_receive.cc
 * @author Alex Pulido
 * @brief Receive and validate packets from the SX1262 transmitting board.
 */

#include <algorithm>
#include <array>

#include "board.h"
#include "test_config.h"

using namespace LBR;

volatile uint32_t packets_received = 0;
volatile uint32_t invalid_packets = 0;
volatile uint32_t receive_failures = 0;

int main()
{
    bsp_init();

    Board& board = get_board();

    Sx1262Params radio_params{
        .settings = sx1262_test_settings(),
        .spi = board.spi,
        .cs = board.cs,
        .reset = board.reset,
        .busy = board.busy,
    };
    Sx1262 radio{radio_params};

    radio.init();

    std::array<uint8_t, 64> rx_buffer{};

    while (1)
    {
        const std::size_t payload_size = radio.receive(rx_buffer);
        if (payload_size == 0)
        {
            ++receive_failures;
            continue;
        }

        const bool payload_matches =
            payload_size == kSx1262TestPayload.size() &&
            std::equal(kSx1262TestPayload.begin(), kSx1262TestPayload.end(),
                       rx_buffer.begin());
        if (payload_matches)
        {
            ++packets_received;
        }
        else
        {
            ++invalid_packets;
        }
    }
}