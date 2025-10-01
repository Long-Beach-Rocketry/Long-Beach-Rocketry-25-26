#include "usart_app_bsp.h"
#include <stdint.h>

StUsart stusart{(uint32_t)4000000, (uint32_t)115200};

Stml4::StGpioSettings config_tx{Stml4::GpioMode::ALT_FUNC,
                                Stml4::GpioOtype::PUSH_PULL,
                                Stml4::GpioOspeed::LOW,
                                Stml4::GpioPupd::NO_PULL,
                                0x7};

Stml4::StGpioSettings config_rx{Stml4::GpioMode::ALT_FUNC,
                                Stml4::GpioOtype::PUSH_PULL,
                                Stml4::GpioOspeed::LOW,
                                Stml4::GpioPupd::NO_PULL,
                                0x7};

Stml4::HwGpio rx_pin(config_rx, (uint8_t)3, GPIOA);

Stml4::HwGpio tx_pin{config_tx, (uint8_t)2, GPIOA};

Usart& BSP_Init(USART_TypeDef* base_addr)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    stusart.init(base_addr, &tx_pin, &rx_pin);
    return stusart;
}
