#include "usart_app_bsp.h"
#include <stdint.h>
// static st_gpio rx_pin(GPIOA, 5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL,
//                       GPIO_SPEED_FREQ_LOW, 0);
// static st_gpio tx_pin(GPIOA, 6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL,
//                       GPIO_SPEED_FREQ_LOW, 0);

StUsart stusart{(uint32_t)4000000, (uint32_t)115200};

Stml4::StGpioSettings config{Stml4::GpioMode::ALT_FUNC,
                             Stml4::GpioOtype::PUSH_PULL,
                             Stml4::GpioOspeed::LOW,
                             Stml4::GpioPupd::NO_PULL,
                             0x7};

Stml4::HwGpio tx_pin{config, (uint8_t)2, GPIOA};

Usart& BSP_Init(USART_TypeDef* base_addr)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    stusart.init(base_addr, &tx_pin);
    return stusart;
}
