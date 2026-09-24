#include "blink_cmd.h"
#include "gpio.h"

namespace LBR
{
Gpio* gpio{nullptr};

namespace BlinkCmd
{

void init(Gpio* led)
{
    gpio = led;
}

bool blink_cmd_handler()
{
    if (gpio)
    {
        return gpio->toggle();
    }
    return false;
}
}  // namespace BlinkCmd

}  // namespace LBR