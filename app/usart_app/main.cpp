#include <stdio.h>
#include "usart_app_bsp.h"

int main(int argc, char** argv)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    uint8_t data[10] = {"testin8\n\r"};
    Usart& usart = BSP_Init(USART2);

    usart.send_tx(data, sizeof(data));

    return 0;
}