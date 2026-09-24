#include "blink_cmd.h"
#include "cli.h"
#include "gpio.h"

namespace LBR
{
void init(Cli& cli, Gpio& led1)
{
    /* TODO: INIT and register */
    BlinkCmd::init(&led1);
}
}  // namespace LBR