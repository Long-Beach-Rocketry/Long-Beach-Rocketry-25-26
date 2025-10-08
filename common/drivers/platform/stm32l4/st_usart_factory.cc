#include "st_usart_factory.h"

StUsart& CreateUsart(uint32_t sys_clck, uint32_t baud_rate,
                     USART_TypeDef* base_addr,
                     LBR::Stml4::StGpioSettings rx_config,
                     LBR::Stml4::StGpioSettings tx_config, uint8_t rx_pin,
                     uint8_t tx_pin, GPIO_TypeDef* rx_base_addr,
                     GPIO_TypeDef* tx_base_addr)
{
    static StUsart st_usart(base_addr, sys_clck, baud_rate);
    LBR::Stml4::HwGpio rx_gpio(rx_config, rx_pin, rx_base_addr);
    LBR::Stml4::HwGpio tx_gpio(tx_config, tx_pin, tx_base_addr);

    st_usart.init();
    rx_gpio.init();
    tx_gpio.init();

    return st_usart;
}

StUsart& GiveUsart(uint32_t sys_clck, uint32_t baud_rate,
                   USART_TypeDef* base_addr,
                   LBR::Stml4::StGpioSettings rx_config,
                   LBR::Stml4::StGpioSettings tx_config, uint8_t rx_pin,
                   uint8_t tx_pin, GPIO_TypeDef* rx_base_addr,
                   GPIO_TypeDef* tx_base_addr)
{
    return CreateUsart(sys_clck, baud_rate, base_addr, rx_config, tx_config,
                       rx_pin, tx_pin, rx_base_addr, tx_base_addr);
}