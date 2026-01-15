#include <cstdio>
#include <cstdlib>
#include "drv8245.h"
#include "board.h"

using namespace LBR;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();
    

    while (1)
    {
        // Drive forward at 50% duty cycle
        hw.motor.setPwm(Drv8245::Direction::Forward, 500);
        for (volatile size_t i = 0; i < 1000000; i++); // Delay

        // Drive reverse at 75% duty cycle
        hw.motor.setPwm(Drv8245::Direction::Reverse, 750);
        for (volatile size_t i = 0; i < 1000000; i++); // Delay

        // Enable coast (Hi-Z)
        hw.motor.enableCoast();
        for (volatile size_t i = 0; i < 1000000; i++); // Delay
        
        // Check for faults
        if (hw.motor.checkFault()) {
            return -1;
        } else {
            return 0;
        }
    }

    return 0;
}