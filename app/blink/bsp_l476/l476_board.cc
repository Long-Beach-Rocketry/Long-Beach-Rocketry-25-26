#include <stdint.h>
#include "board.h"

#include "st_gpio.h"

namespace LBR
{
namespace Board
{
Stml4::StGpioSettings led_cfg{
    Stml4::GpioMode::GPOUT, Stml4::GpioOtype::PUSH_PULL, Stml4::GpioOspeed::LOW,
    Stml4::GpioPupd::NO_PULL, 0};
Stml4::HwGpio led{led_cfg, static_cast<uint8_t>(5), GPIOA};

bool init()
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    led.init();
    return true;
}

Gpio& get_led()
{
    return led;
}
}  // namespace Board
}  // namespace LBR