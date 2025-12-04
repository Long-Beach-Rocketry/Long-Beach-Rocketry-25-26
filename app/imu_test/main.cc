#include <cstdlib>
#include <span>
#include "board.h"

using namespace LBR;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    (void)hw.imu.read_reg(LBR::Bno055::SysReg::CHIP_ID);

    while (1)
    {
        // Simple busy-wait delay (approximate)
        for (volatile size_t i = 0; i < 100000; i++);
    }

    return 0;
}