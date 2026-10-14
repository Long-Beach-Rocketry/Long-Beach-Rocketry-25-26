#include <cstdio>
#include "board.h"
#include "delay.h"

using namespace LBR;

int main(int argc, char* argv[])
{
    if (!bsp_init())
    {
        printf("MS5611 initialization failed\n");
        return 1;
    }

    Board& hw = get_board();

    while (1)
    {
        float press = hw.barometer.get_pressure();
        float temp = hw.barometer.get_temperature();

        printf("Pressure: %f Pa\nTemperature: %f C\n", press, temp);
        Utils::DelayMs(800);
    }

    return 0;
}
