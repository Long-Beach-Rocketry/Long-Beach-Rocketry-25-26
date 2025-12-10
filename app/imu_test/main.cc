


#include "board.h"
#include "bno055_imu.h"

using namespace LBR;

int main(int argc, char* argv[]) {
    
    bsp_init();
    Board hw = get_board();

    Bno055Data data;
    uint8_t chip_id = 0;
    uint8_t calib_status = 0;
    uint8_t opr_mode = 0;
    uint8_t sys_status = 0;
    uint8_t self_test = 0;
    uint8_t sys_error = 0;

    hw.imu.read_all(data);
    hw.imu.get_chip_id(chip_id);
    hw.imu.calibrate(calib_status);
    hw.imu.get_opr_mode(opr_mode);
    hw.imu.get_sys_status(sys_status);
    hw.imu.get_sys_error(sys_error);
    hw.imu.run_post(self_test);

    while (1) {
        (void)data;
        (void)chip_id;
        (void)calib_status;
        (void)opr_mode;
        (void)sys_status;
        (void)sys_error;
        (void)self_test;
        for (volatile size_t i = 0; i < 100000; i++);
    }
    return 0;
}