#include <array>
#include <cstddef>
#include "board.h"
#include "cli.h"
#include "cli_config.h"
#include "delay.h"

using namespace LBR;

// LBR::RingBuffer<char, 128> rxBuffer;
uint8_t rxb;
bool update_flag{false};
Cli cli("Cli app> ");

int main(int argc, char* argv[])
{
    board_init();
    Board& hw = get_board();

#ifdef STM32H723xx
    cli_init(cli, &hw.usart, hw.led1, hw.led2, hw.led3, nullptr);

#elif defined(STM32L476xx)
    cli_init(cli, &hw.usart, hw.led1);

#endif

    cli.list_commands();
    hw.usart.send(cli.get_prompt());

    while (1)
    {
        // hw.led1.toggle();
        // Utils::DelayMs(1000);
        if (update_flag)
        {
            update_flag = false;
            cli.process();
            hw.usart.send(cli.get_prompt());
        }
    }

    return 0;
}