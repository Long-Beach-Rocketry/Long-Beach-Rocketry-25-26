#include <cstdlib>
#include "board.h"
#include "gpio.h"
// extern "C"
// {
// }

using namespace LBR;

int main(int argc, char* argv[])
{
    board_init();
    Board hw = get_board();
    while (1)
    {
        hw.led.toggle();
        for (volatile size_t i = 0; i < 100000; i++);  // Delay
    }

    return 0;
}
