#include "usart_app_bsp.h"

int main(int argc, char** argv)
{
    uint8_t str_arr[] = "Message Recieved";
    uint8_t data[] = "";

    if (BSP_Init(USART2).receive_rx(data))
    {
        BSP_Init(USART2).send_tx(str_arr, sizeof(str_arr));
    }

    while (1);

    return 0;
}