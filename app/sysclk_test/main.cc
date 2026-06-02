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
        // print frequencies to serial monitor
        char buf[100];
        int len =
            snprintf(buf, sizeof(buf),
                     "SYSCLK: %lu Hz, HCLK: %lu Hz, PCLK1: %lu Hz, PCLK2: %lu "
                     "Hz, PCLK3: %lu Hz, PCLK4: %lu Hz\r\n",
                     (unsigned long)freqs.sysclk, (unsigned long)freqs.ahb,
                     (unsigned long)freqs.apb1, (unsigned long)freqs.apb2,
                     (unsigned long)freqs.apb3, (unsigned long)freqs.apb4);
        std::span<const uint8_t> tx_span((uint8_t*)buf, len);
        board.usart.send(tx_span);

        // Busy wait
        Utils::DelayMs(1000);
    }

    return 0;
}