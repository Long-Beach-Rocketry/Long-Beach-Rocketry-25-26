#include <array>
#include <cstddef>
#include "board.h"
#include "cli.h"
#include "cli_config.h"
#include "delay.h"

using namespace LBR;

uint8_t rxb;
bool update_flag{false};
Cli cli("Cli app> ");

uint8_t chip_id = 0;
Bno055::Mode opr_mode = Bno055::Mode::CONFIG;
uint8_t sys_status = 0;
uint8_t self_test = 0;
uint8_t sys_error = 0;

int main(int argc, char* argv[])
{
    board_init();
    Board& hw = get_board();

    hw.bno055.get_chip_id(chip_id);
    hw.bno055.get_opr_mode(opr_mode);
    hw.bno055.get_sys_status(sys_status);
    hw.bno055.run_post(self_test);
    hw.bno055.get_sys_error(sys_error);

#ifdef STM32H723xx
    cli_init(cli, &hw.usart, hw.led1, hw.led2, hw.led3, &hw.bno055, nullptr);

#elif defined(STM32L476xx)
    cli_init(cli, &hw.usart, hw.led1);

#endif

    cli.list_commands();
    hw.usart.send(cli.get_prompt());

    while (1)
    {
        if (update_flag)
        {
            update_flag = false;
            cli.process();
            hw.usart.send(cli.get_prompt());
        }
    }

    return 0;
}