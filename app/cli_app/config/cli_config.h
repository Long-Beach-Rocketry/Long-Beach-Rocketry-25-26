#include "blink_cmd.h"
#include "cli.h"
#include "gpio.h"
#include "usart.h"

namespace LBR
{
#ifdef STM32H723xx

void cli_init(Cli& cli, Usart* usart, Gpio& led1, Gpio& led2, Gpio& led3)
{
    cli.init(usart);
    /* TODO: INIT and register */
    BlinkCmd::init(&led1, &led2, &led3);
    cli.register_cmd(BlinkCmd::blinkCmd);
}

#elif defined(STM32L476xx)
void cli_init(Cli& cli, Usart* usart, Gpio& led1)
{
    cli.init(usart);
    /* TODO: INIT and register */
    BlinkCmd::init(&led1, nullptr, nullptr);
    cli.register_cmd(BlinkCmd::blinkCmd);
}
#endif

}  // namespace LBR