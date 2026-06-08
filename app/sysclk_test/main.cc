#include <cstdio>
#include "board.h"
#include "delay.h"

using namespace LBR;

uint8_t rxb;

int main(int argc, char** argv)
{
    bsp_init();

    Board& board = get_board();

    while (1)
    {
        const ClockFrequencies& freqs = board.clock.get_clock_frequencies();
        constexpr double kHzPerMHz = 1'000'000.0;

        char buf[320];
        int len = snprintf(buf, sizeof(buf),
                           "--------------------------------\r\n"
                           " CLOCK TREE\r\n"
                           "--------------------------------\r\n"
                           " SYSCLK : %.2f MHz\r\n"
                           " CPU    : %.2f MHz\r\n"
                           " HCLK   : %.2f MHz\r\n"
                           " PCLK1  : %.2f MHz\r\n"
                           " PCLK2  : %.2f MHz\r\n"
                           " PCLK3  : %.2f MHz\r\n"
                           " PCLK4  : %.2f MHz\r\n"
                           "--------------------------------\r\n",
                           freqs.sysclk / kHzPerMHz, freqs.cpu / kHzPerMHz,
                           freqs.ahb / kHzPerMHz, freqs.apb1 / kHzPerMHz,
                           freqs.apb2 / kHzPerMHz, freqs.apb3 / kHzPerMHz,
                           freqs.apb4 / kHzPerMHz);

        if (len < 0)
        {
            continue;
        }

        if (len >= static_cast<int>(sizeof(buf)))
        {
            len = static_cast<int>(sizeof(buf)) - 1;
        }

        std::span<const uint8_t> tx_span(reinterpret_cast<const uint8_t*>(buf),
                                         static_cast<size_t>(len));
        board.usart.send(tx_span);

        // Busy wait
        Utils::DelayMs(2500);
    }

    return 0;
}