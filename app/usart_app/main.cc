#include "board.h"

uint8_t rxb[16] = "Message";
std::span<uint8_t> tx_span(rxb, sizeof(rxb));

uint8_t data[64];
std::span<uint8_t> rx_span(data, sizeof(data));

int main(int argc, char** argv)
{
    BSP_Init();

    Board board = get_board();

    board.usart.send_tx(tx_span);

    while (1);

    return 0;
}
