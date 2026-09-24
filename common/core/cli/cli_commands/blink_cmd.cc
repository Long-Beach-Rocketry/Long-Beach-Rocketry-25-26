#include "blink_cmd.h"
#include "gpio.h"

namespace LBR
{
static Gpio* gpio{nullptr};

namespace BlinkCmd
{

void init(Gpio* led)
{
    gpio = led;
}

bool blink_cmd_handler(const CmdArgs& args)
{
    if (gpio)
    {
        return gpio->toggle();
    }
    return false;
}

// const Cmd blinkCmd = {.name = "blink",
//                       .desc = "Toggles the assigned status GPIO pin",
//                       .invoke = BlinkCmd::blink_cmd_handler};

}  // namespace BlinkCmd

}  // namespace LBR