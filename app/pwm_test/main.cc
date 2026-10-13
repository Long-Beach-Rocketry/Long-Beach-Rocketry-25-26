#include <cstddef>
#include "board.h"
#include "servo_motor.h"

using namespace LBR;

int main()
{
    if (!bsp_init())
    {
        return 1;
    }

    Board& hw = get_board();

    ServoMotorParams params{hw.pwm, 1390, 1610, 30, 50};
    ServoMotor servo{params};

    if (!servo.init())
    {
        return 1;
    }

    while (1)
    {
        servo.set_angle(0);
        for (volatile size_t i = 0; i < 4000000; ++i)
        {
        }

        servo.set_angle(15);
        for (volatile size_t i = 0; i < 4000000; ++i)
        {
        }

        servo.set_angle(30);
        for (volatile size_t i = 0; i < 4000000; ++i)
        {
        }
    }
}