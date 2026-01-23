#include "board.h"

using namespace LBR;

std::array<uint8_t, 17> txb{"i <3 embedded\r\n"};
uint8_t rxb;

int main(int argc, char** argv)
{
    BSP_Init();

    Board board = get_board();

    while (1)
    {
        board.usart.send_tx(txb);

        // Busy wait
        for (volatile uint32_t i = 0; i < 1000000; i++);
    }

    return 0;
}
