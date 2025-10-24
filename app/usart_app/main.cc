#include "usart_app_bsp.h"

uint8_t rxb[] = "Message";
uint8_t dat[20];
// // uint8_t data = "message";

// static Board board = get_board();
Usart& usart = BSP_Init(USART2);

int main(int argc, char** argv)
{

    // BSP_Init(USART2);

    // uint8_t str_arr[] = "";
    // uint8_t data[] = {};

    usart.send_tx(rxb, sizeof(rxb));

    NVIC_SetPriority(USART2_IRQn, 0);
    NVIC_EnableIRQ(USART2_IRQn);

    // usart.receive_rx(data);

    while (1);

    return 0;
}

extern "C" void USART2_IRQHandler(void)
{

    //ADD SIZE PARAMTER!!
    usart.receive_rx(dat, sizeof(dat));

    uint8_t temp[20];

    for (size_t i = 0; i < sizeof(dat); i++)
    {
        temp[i] = ++dat[i];
    }

    // uint8_t arr[] = {temp};

    usart.send_tx(temp, sizeof(temp));
    // USART2->ISR &= ~USART_ISR_RXNE;
}
