#include "usart_app_bsp.h"
#include <stdint.h>

Usart& BSP_Init(USART_TypeDef* base_addr)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    return GiveUsart((uint32_t)4000000,
                     (uint32_t)115200,
                     base_addr,
                     {Stml4::GpioMode::ALT_FUNC,
                      Stml4::GpioOtype::PUSH_PULL,
                      Stml4::GpioOspeed::LOW,
                      Stml4::GpioPupd::NO_PULL,
                      0x7},
                     {Stml4::GpioMode::ALT_FUNC,
                      Stml4::GpioOtype::PUSH_PULL,
                      Stml4::GpioOspeed::LOW,
                      Stml4::GpioPupd::NO_PULL,
                      0x7},
                     (uint8_t)3,
                     (uint8_t)2,
                     GPIOA,
                     GPIOA);
}
