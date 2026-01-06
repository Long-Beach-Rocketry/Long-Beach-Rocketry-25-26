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
Bno055::Mode opr_mode = Bno055::Mode::CONFIG;
uint8_t sys_status = 0;
uint8_t self_test = 0;
uint8_t sys_error = 0;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    hw.imu.get_chip_id(chip_id);
    hw.imu.get_opr_mode(opr_mode);
    hw.imu.get_sys_status(sys_status);
    hw.imu.run_post(self_test);
    hw.imu.get_sys_error(sys_error);

    while (1)
    {
        hw.imu.read_all(data);
        (void)data;
    }
    return 0;
}