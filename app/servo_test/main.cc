#include <cstdlib>
#include "board.h"
#include "servo_motor.h"

using namespace LBR;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();
    uint8_t angle = 0;

    while (1)
    {
        angle += 10;
        if (angle > 180)
        {
            angle = 0;
        }

        hw.servo_motor.set_angle(angle);

        for (volatile size_t i = 0; i < 200000; i++);
    }

    return 0;
}