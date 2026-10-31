#include <cstdio>
#include <cstdlib>
#include <span>
#include "board.h"
#include "delay.h"
#include "ms561101.h"

using namespace LBR;
float temp, press;
volatile bool flag_press{false};
volatile bool flag_temp{false};
int main(int argc, char* argv[])
{
    if (!bsp_init())
    {
        printf("MS5611 initialization failed\r\n");
    }
    Board hw = get_board();

    while (1)
    {
        flag_press = hw.barometer.get_pressure(press);
        flag_temp = hw.barometer.get_temperature(temp);
    }
    //format: ./format.sh, ./format_check.sh or ctrl+S
    return 0;
}