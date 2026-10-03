#include <cstdio>
#include <cstdlib>
#include <span>
#include "bmp390.h"
#include "board.h"
#include "delay.h"

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

        printf("Pressure: %f\nTemperature: %f\n", press, temp);

        Utils::DelayMs(800);
    }

    return 0;
}