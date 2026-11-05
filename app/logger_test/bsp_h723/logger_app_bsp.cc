#include "board.h"
#include "st_gpio.h"
#include "st_usart.h"

using namespace LBR::Stmh7;

namespace LBR
{
namespace Stmh7
{

StGpioSettings tx_config = {GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                            GpioOspeed::LOW, GpioPupd::NO_PULL, 0x7};

StGpioSettings rx_config = {GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                            GpioOspeed::LOW, GpioPupd::NO_PULL, 0x7};

StGpioParams tx_params = {tx_config, (uint8_t)8, GPIOD};
StGpioParams rx_params = {rx_config, (uint8_t)9, GPIOD};

HwGpio tx_gpio(tx_params);
HwGpio rx_gpio(rx_params);

// Default frequency for the H7 (HSI / 1), (per Datasheet 8.7.2)
StUsartParams usart_params = {USART3, 64000000, 115200};

StUsart usart(usart_params);
}  // namespace Stmh7

Board board{.usart = Stmh7::usart, .rx = Stmh7::rx_gpio, .tx = Stmh7::tx_gpio};

bool bsp_init()
{
    bool result = true;

    // Enable GPIOD for pins PD8, PD9
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;

    // Enable USART3 clock
    RCC->APB1LENR |= RCC_APB1LENR_USART3EN;

    result &= tx_gpio.init();

    result &= rx_gpio.init();

    result &= usart.init();
    return result;
}

Board& get_board()
{
    return board;
}
}  // namespace LBR