#include <cstdio>
#include <cstdlib>
#include <span>
#include "ms561101.h"
#include "board.h"
#include "delay.h"

using namespace LBR;
float temp, press;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    while (1)
    {
        press = hw.barometer.get_pressure();
        temp = hw.barometer.get_temperature();
        //Utils::DelayMs(800);
    }

    return 0;
}