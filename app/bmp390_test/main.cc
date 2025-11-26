#include <cstdlib>
#include <span>
#include "bmp390.h"
#include "board.h"

using namespace LBR;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();
    float temp, press;

    while (1)
    {
        press = hw.bmp390.get_pressure();
        temp = hw.bmp390.get_temperature();

        for (volatile size_t i = 0; i < 100000; i++);
    }

    return 0;
}