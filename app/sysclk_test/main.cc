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
        constexpr uint32_t kHzPerMHz = 1'000'000U;

        char buf[320];
        int len = snprintf(buf, sizeof(buf),
                           "--------------------------------\r\n"
                           " SYSCLK TEST\r\n"
                           "--------------------------------\r\n"
                           " SYSCLK : %10f MHz\r\n"
                           " CPU    : %10f MHz\r\n"
                           " HCLK   : %10f MHz\r\n"
                           " PCLK1  : %10f MHz\r\n"
                           " PCLK2  : %10f MHz\r\n"
                           " PCLK3  : %10f MHz\r\n"
                           " PCLK4  : %10f MHz\r\n"
                           "--------------------------------\r\n",
                           static_cast<float>(freqs.sysclk / kHzPerMHz),
                           static_cast<float>(freqs.cpu / kHzPerMHz),
                           static_cast<float>(freqs.ahb / kHzPerMHz),
                           static_cast<float>(freqs.apb1 / kHzPerMHz),
                           static_cast<float>(freqs.apb2 / kHzPerMHz),
                           static_cast<float>(freqs.apb3 / kHzPerMHz),
                           static_cast<float>(freqs.apb4 / kHzPerMHz));

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
        Utils::DelayMs(1000);
    }

    return 0;
}