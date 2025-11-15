#include "board.h"

uint8_t txb[] = {"Message"};

uint8_t rxb;

int main(int argc, char** argv)
{
    BSP_Init();

    Board board = get_board();

    board.usart.send_tx(*txb);

    while (1)
    {
    }

    return 0;
}
