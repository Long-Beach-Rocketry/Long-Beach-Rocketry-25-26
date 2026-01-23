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
        // Set motor speed
        hw.motor.setSpeed(0.5f);  // Set speed to 50%
        // Set Direction 
        hw.motor.setDirection(Drv8245::Direction::Forward);
        for (volatile int i = 0; i < 1000000; ++i);  // Delay

        hw.motor.setDirection(Drv8245::Direction::Reverse);
        for (volatile int i = 0; i < 1000000; ++i);  // Delay

        hw.motor.setSpeed(0.0f);  // Stop motor
        for (volatile int i = 0; i < 1000000; ++i);  // Delay

        hw.motor.checkFault();  // Check for faults
        
    }

    return 0;
}