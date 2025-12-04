#include <cstdlib>
#include <span>
#include "board.h"

using namespace LBR;

volatile uint8_t chip_id = 0; // Global for debugger visibility

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    // Read chip ID and store in chip_id
    uint8_t id = hw.imu.read_reg(LBR::Bno055::SysReg::CHIP_ID);
    chip_id = id;

    // Stop point for debugger (infinite loop)
    while (1)
    {
        // Simple busy-wait delay (approximate)
        for (volatile size_t i = 0; i < 100000; i++);
    }

    return 0;
}