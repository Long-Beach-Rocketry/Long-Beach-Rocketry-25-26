#include "usart_app_bsp.h"
#include <stdint.h>

Usart& BSP_Init(USART_TypeDef* base_addr)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    LBR::Stml4::StGpioSettings rx_config = {
        LBR::Stml4::GpioMode::ALT_FUNC, LBR::Stml4::GpioOtype::PUSH_PULL,
        LBR::Stml4::GpioOspeed::LOW, LBR::Stml4::GpioPupd::NO_PULL, 0x7};
    LBR::Stml4::StGpioSettings tx_config = {
        LBR::Stml4::GpioMode::ALT_FUNC, LBR::Stml4::GpioOtype::PUSH_PULL,
        LBR::Stml4::GpioOspeed::LOW, LBR::Stml4::GpioPupd::NO_PULL, 0x7};
    LBR::Stml4::StGpioParams rx_params = {rx_config, (uint8_t)3, GPIOA};
    LBR::Stml4::StGpioParams tx_params = {tx_config, (uint8_t)2, GPIOA};

    StUsartParams usart_params = {USART2, 4000000, 115200};

    return UsartModule(usart_params, rx_params, tx_params);
}

Board board{BSP_Init(USART2)};

Board& get_board(void)
{
    return board;
}
