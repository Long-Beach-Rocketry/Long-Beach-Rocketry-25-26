#include <stdint.h>
#include "board.h"
#include "st_gpio.h"
#include "st_usart.h"

using namespace LBR::Stml4;
namespace LBR
{
LBR::Stml4::StGpioSettings rx_config = {
    LBR::Stml4::GpioMode::ALT_FUNC, LBR::Stml4::GpioOtype::PUSH_PULL,
    LBR::Stml4::GpioOspeed::LOW, LBR::Stml4::GpioPupd::NO_PULL, 0x7};
LBR::Stml4::StGpioSettings tx_config = {
    LBR::Stml4::GpioMode::ALT_FUNC, LBR::Stml4::GpioOtype::PUSH_PULL,
    LBR::Stml4::GpioOspeed::LOW, LBR::Stml4::GpioPupd::NO_PULL, 0x7};
LBR::Stml4::StGpioParams rx_params = {rx_config, (uint8_t)3, GPIOA};
LBR::Stml4::StGpioParams tx_params = {tx_config, (uint8_t)2, GPIOA};

LBR::Stml4::HwGpio rx_gpio(rx_params);
LBR::Stml4::HwGpio tx_gpio(tx_params);

StUsart usart(USART2, 4000000, 9600);

Board board{.usart = usart, .rx = rx_gpio, .tx = tx_gpio};

bool BSP_Init()
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN | RCC_APB1ENR1_USART3EN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    if (!usart.init())
    {
        return false;
    }
    if (!rx_gpio.init())
    {
        return false;
    }
    if (!tx_gpio.init())
    {
        return false;
    }

    NVIC_SetPriority(USART2_IRQn, 0);
    NVIC_EnableIRQ(USART2_IRQn);

    return true;
}

Board& get_board()
{
    return board;
}

extern "C" void USART2_IRQHandler(void)
{
    if (board.usart.receive_rx_nb(rxb))
    {
        // Received 1 byte, echo it back
        std::span<const uint8_t> tx_span(&rxb, 1);
        board.usart.send_tx(tx_span);
    }
}
}  // namespace LBR