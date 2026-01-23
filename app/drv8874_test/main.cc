#include <cstdio>
#include <cstdlib>
#include "board.h"
#include "drv8874.h"

using namespace LBR;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    while (1)
    {
        // Drive forward at 50% duty cycle
        hw.motor.setSpeed(50);
        hw.motor.setDirection(Drv8874::Direction::FORWARD);
        for (volatile size_t i = 0; i < 1000000; i++);

        // Enable brake then coast
        hw.motor.enableBrake();
        for (volatile size_t i = 0; i < 1000000; i++);
        hw.motor.enableCoast();
        for (volatile size_t i = 0; i < 1000000; i++);

        // setSleep false (sleep mode)
        hw.motor.setSleep(false);
        for (volatile size_t i = 0; i < 1000000; i++);

        // Check for faults
        if (hw.motor.checkFault())
        {
            return -1;
        }
        else
        {
            return 0;
        }

        // Drive backward at 75% duty cycle
        hw.motor.setSpeed(-75);
        hw.motor.setSleep(true);  // Wake up
        for (volatile size_t i = 0; i < 1000000; i++);
    }

    return 0;
}