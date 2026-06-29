#include <array>
#include <cstdint>
#include <cstdio>
#include "board.h"

using namespace LBR;

std::array<uint8_t, 17> txb{'i', ' ', '<', '3', ' ',  'e',  'm', 'b', 'e',
                            'd', 'd', 'e', 'd', '\r', '\n', 0,   0};

int main(int argc, char** argv)
{
    bsp_init();
    Board& board = get_board();
    board.rs485.init();

    while (1)
    {
        board.rs485.set_direction(
            Rs485::Direction::LOOPBACK);  // PA4 goes HIGH, PA5 goes LOW
        board.usart.send(txb);
        board.rs485.set_direction(Rs485::Direction::RECEIVE);

        for (volatile uint32_t delay = 0; delay < 2000; delay++);

        uint8_t inbound_char = 0;
        while (board.usart.receive(inbound_char))
        {
            if (inbound_char >= 32 && inbound_char <= 126)
            {
                printf("%c", (char)inbound_char);
            }
        }
        printf("\r\n");

        for (volatile uint32_t i = 0; i < 2000000; i++)
        {
        }
    }

    return 0;
}