#include <cstdio>
#include <cstdlib>
#include <span>
#include "board.h"
#include "delay.h"
#include "ms561101.h"

using namespace LBR;
float temp, press;
bool flag_press;
bool flag_temp;
int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    while (1)
    {
        flag_press = hw.barometer.get_pressure(press);
        flag_temp = hw.barometer.get_temperature(temp);
        //press = hw.barometer.get_pressure(press);
        //temp = hw.barometer.get_temperature(temp);
    }
    //format: ./format.sh, ./format_check.sh or ctrl+S
    return 0;
}