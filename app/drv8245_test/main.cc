#include "board.h"

using namespace LBR;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    // Initial test: set direction FORWARD and speed to 50%
    hw.drv8245.set_direction(Drv8245::Direction::FORWARD);
    hw.drv8245.set_speed(50);

    while (1)
    {
    }

    return 0;
}