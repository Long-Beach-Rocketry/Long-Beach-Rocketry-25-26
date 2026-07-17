#include "board.h"
#include "rs485.h"
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

StGpioParams tx_params = {tx_config, (uint8_t)5, GPIOD};  // PD5
StGpioParams rx_params = {rx_config, (uint8_t)6, GPIOD};  // PD6

HwGpio tx_gpio(tx_params);
HwGpio rx_gpio(rx_params);

// DE and RE controls configured on GPIOA
StGpioSettings control_pin_config = {GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                                     GpioOspeed::LOW, GpioPupd::NO_PULL, 0x0};

StGpioParams de_params = {control_pin_config, (uint8_t)4,
                          GPIOA};  // PA4 tied to DE (Pin 3)
StGpioParams re_params = {control_pin_config, (uint8_t)5,
                          GPIOA};  // PA5 tied to RE (Pin 2)

HwGpio de_gpio(de_params);
HwGpio re_gpio(re_params);

Rs485 rs485_driver(de_gpio, re_gpio);

StUsartParams usart_params = {USART2, 64000000, 115200};
StUsart usart(usart_params);
}  // namespace Stmh7

Board board{.usart = Stmh7::usart,
            .rx = Stmh7::rx_gpio,
            .tx = Stmh7::tx_gpio,
            .rs485 = Stmh7::rs485_driver};

bool bsp_init()
{
    bool result = true;

    // Turn on the peripheral clock for Port A AND Port D
    RCC->AHB4ENR |= (RCC_AHB4ENR_GPIOAEN | RCC_AHB4ENR_GPIODEN);
    RCC->APB1LENR |= RCC_APB1LENR_USART2EN;

    RCC->D2CCIP2R &= ~RCC_D2CCIP2R_USART28SEL;

    // Initialize all hardware blocks
    result &= Stmh7::de_gpio.init();
    result &= Stmh7::re_gpio.init();
    result &= Stmh7::tx_gpio.init();
    result &= Stmh7::rx_gpio.init();
    result &= Stmh7::usart.init();

    return result;
}

Board& get_board()
{
    return board;
}
}  // namespace LBR