#include <array>
#include <cstddef>
#include "board.h"
#include "cli.h"
#include "delay.h"

using namespace LBR;

// LBR::RingBuffer<char, 128> rxBuffer;
uint8_t rxb;
bool update_flag{false};

int main(int argc, char* argv[])
{
    board_init();
    Board& hw = get_board();

    //  Gpio* ld1 = &hw.led1;
    //  CliParams params{ld1, nullptr, nullptr, nullptr, nullptr};
    //  Cli cli(params);
    //  cli.init();

    while (1)
    {
        hw.led1.toggle();
        Utils::DelayMs(1000);
        if (update_flag)
        {
            update_flag = false;
            // cli.update(rxb);
        }
    }

    return 0;
}