/**
 * @file main.cc
 * @brief IMU Test Application Main File
 * @author Bex Saw
 * @date 2025-10-21
 */

#include "bno055_imu.h"
#include "board.h"

using namespace LBR;

Bno055Data data;
uint8_t chip_id = 0;
uint8_t opr_mode = 0;
uint8_t sys_status = 0;
uint8_t self_test = 0;
uint8_t sys_error = 0;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    while (1)
    {
        hw.imu.read_all(data);

        (void)data;
    }
    return 0;
}