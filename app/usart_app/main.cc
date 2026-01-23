#include "board.h"

using namespace LBR;

std::array<uint8_t, 17> txb{"i <3 embedded\r\n"};
uint8_t rxb;

int main(int argc, char** argv)
{
    BspInit();

    Board board = GetBoard();

    while (1)
    {
        board.usart.Send(txb);

        // Busy wait
        for (volatile uint32_t i = 0; i < 1000000; i++);
    }

    return 0;
}
