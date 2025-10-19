#include "st_usart_module.h"

Usart& UsartModule(StUsartParams& usart_params, LBR::Stml4::StGpioParams& rx,
                   LBR::Stml4::StGpioParams& tx)
{
    static StUsart st_usart(usart_params.base_addr, usart_params.sys_clck,
                            usart_params.baud_rate);

    LBR::Stml4::HwGpio rx_gpio(rx);
    LBR::Stml4::HwGpio tx_gpio(tx);

    st_usart.init();
    rx_gpio.init();
    tx_gpio.init();

    return st_usart;
}