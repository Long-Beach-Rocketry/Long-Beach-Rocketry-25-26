#include <cstdlib>
#include "board.h"
#include "pwm.h"

using namespace LBR;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    while (1)
    {
        hw.pwm.set_freq(20000);
        hw.pwm.set_duty_cycle(25);

        for (volatile size_t i = 0; i < 4000000; i++);

        hw.pwm.set_freq(50000);
        hw.pwm.set_duty_cycle(100);

        for (volatile size_t i = 0; i < 4000000; i++);
    }

    return 0;
}