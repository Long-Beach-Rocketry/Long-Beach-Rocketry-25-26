#include "blink_cmd.h"
#include <array>
#include "gpio.h"

namespace LBR
{
static std::array<Gpio*, 3> gpios{nullptr, nullptr, nullptr};

namespace BlinkCmd
{

void init(Gpio* led1, Gpio* led2, Gpio* led3)
{
    gpios[0] = led1;
    gpios[1] = led2;
    gpios[2] = led3;
}

bool blink_cmd_handler(const CmdArgs& args)
{
    // should parse args so it does specified led
    bool ret{false};
    for (auto gpio : gpios)
    {
        if (gpio)
        {
            ret |= gpio->toggle();
        }
    }
    return ret;
}

const Cmd blinkCmd = {.name = "blink",
                      .desc = "Toggles the led of a GPIO pin",
                      .invoke = blink_cmd_handler};

}  // namespace BlinkCmd

}  // namespace LBR