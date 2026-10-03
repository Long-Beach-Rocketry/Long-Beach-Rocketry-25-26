#include "blink_cmd.h"
#include "cli.h"
#include "gpio.h"
#include "sensor_cmd.h"
#include "usart.h"

namespace LBR
{
#ifdef STM32H723xx

void cli_init(Cli& cli, Usart* usart, Gpio& led1, Gpio& led2, Gpio& led3,
              Bmp390* baro)
{
    cli.init(usart);
    /* Init Commands here */
    BlinkCmd::init(&led1, &led2, &led3);
    SensorCmd::init(nullptr, baro);

    /* Register Commands here */
    cli.register_cmd(BlinkCmd::blinkCmd);
    cli.register_cmd(SensorCmd::sensorCmd);
}
/* This ifdef is mainly because H7 has 3 controllable led gpios while L4 has 1 */
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