#include "usart_app_bsp.h"

int main(int argc, char** argv)
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    uint8_t data[] = {"testin8"};
    // Usart& usart = BSP_Init(USART2);

    // usart.send_tx(data, sizeof(data));

    BSP_Init(USART2).send_tx(data, sizeof(data));

    while (1);

    return 0;
}