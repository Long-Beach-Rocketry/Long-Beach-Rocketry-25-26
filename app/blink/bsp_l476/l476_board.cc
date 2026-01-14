#include <stdint.h>
#include "board.h"

#include "st_gpio.h"
#include "st_sys_clock.h"

namespace LBR
{
Stml4::StGpioSettings led_settings{
    Stml4::GpioMode::GPOUT, Stml4::GpioOtype::PUSH_PULL, Stml4::GpioOspeed::LOW,
    Stml4::GpioPupd::NO_PULL, 0};

Stml4::StGpioParams led_params{
    led_settings,
    static_cast<uint8_t>(5),
    GPIOA,
};

Stml4::HwGpio led{led_params};

Board board{.led = led};

Stml4::HwClock clock{};

bool board_init()
{
    bool return_val = true;

    return_val &= clock.init();

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    return_val &= led.init();

    return return_val;
}

Board& get_board(void)
{
    return board;
}

}  // namespace LBR