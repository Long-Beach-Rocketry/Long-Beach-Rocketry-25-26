#include "usart_app_bsp.h"

int main(int argc, char** argv)
{
    uint8_t data[] = {"Received Message Successfully"};
    uint8_t recieve_data[1] = {};

    if (BSP_Init(USART2).receive_rx(recieve_data, sizeof(recieve_data)))
    {
        BSP_Init(USART2).send_tx(data, sizeof(data));
    }

    while (1);

    return 0;
}