#include <span>
#include "board.h"

std::array<uint8_t, 7> txb{'m', 'e', 's', 's', 'a', 'g', 'e'};

std::array<uint8_t, 7> rxb;

int main(int argc, char** argv)
{
    BSP_Init();

    Board board = get_board();

    while (1)
    {
        board.usart.send_tx(txb);
    }

    return 0;
}
