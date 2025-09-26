#include "usart_app_bsp.h"
// static st_gpio rx_pin(GPIOA, 5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL,
//                       GPIO_SPEED_FREQ_LOW, 0);
// static st_gpio tx_pin(GPIOA, 6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL,
//                       GPIO_SPEED_FREQ_LOW, 0);

StUsart stusart(80000000, 115200);

Usart& BSP_Init(USART_TypeDef* base_addr)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    stusart.init(base_addr);
    return stusart;
}

// Gpio& BSP_GPIO_Init(GPIO_TypeDef* base_addr, uint8_t pin, uint32_t mode,
//                     uint32_t pull, uint32_t speed, uint32_t af)
// {
//     RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

//     return StGpio;
// }