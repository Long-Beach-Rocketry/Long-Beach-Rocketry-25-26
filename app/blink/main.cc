#include "board.h"
#include "gpio.h"

// extern "C"
// {
// // #include "stm32l476xx.h"
// // #include "stm32l4xx.h"
// #include "stm32l4xx_hal.h"
// }

using namespace LBR;

// Im gonna make everything here for now in main goal is to try to get this done today
int main(int argc, char* argv[])
{
    Board::init();
    while (1)
    {
        Board::get_led().set(true);
        for (volatile int i = 0; i < 100000; i++);  // Delay
        Board::get_led().set(false);
        for (volatile int i = 0; i < 100000; i++);  // Delay
    }

    return 0;
}
