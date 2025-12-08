

#include "board.h"

using namespace LBR;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    // Test 1: Read CHIP_ID
    uint8_t chip_id = hw.imu.read_reg(LBR::Bno055::SysReg::CHIP_ID); // Expected 0xA0
    uint8_t accel_id = hw.imu.read_reg(0x01); // Expected accel ID 0xFB
    uint8_t gyro_id = hw.imu.read_reg(0x03);  // Expected gyro ID 0x0F

    // Diagnostics: Print OPR_MODE and PAGE_ID before mode switch
    uint8_t opr_mode_before = hw.imu.read_reg(LBR::Bno055::SysReg::OPR_MODE);
    uint8_t page_id_before = hw.imu.read_reg(LBR::Bno055::SysReg::PAGE_ID);
 

    // Test 2: Set IMU mode and check OPR_MODE
    hw.imu.set_mode(LBR::Bno055::Mode::IMU);

    // Diagnostics: Print OPR_MODE and PAGE_ID after mode switch
    uint8_t opr_mode_after = hw.imu.read_reg(LBR::Bno055::SysReg::OPR_MODE);
    uint8_t page_id_after = hw.imu.read_reg(LBR::Bno055::SysReg::PAGE_ID);
   
    // Test 3: Read accel and gyro registers (raw)
    uint8_t accel[6] = {0};
    hw.imu.read_multi(LBR::Bno055::RegStart::ACC, accel, 6);
    uint8_t gyro[6] = {0};
    hw.imu.read_multi(LBR::Bno055::RegStart::GYR, gyro, 6);

    // Test 4: Read sys_error and sys_status
    uint8_t sys_status = hw.imu.get_sys_status();
    uint8_t sys_error = hw.imu.get_sys_error();

    // Test 5: Run self-test (POST)
    uint8_t self_test = hw.imu.run_post();

    while (1) {
        (void)chip_id;
        (void)accel_id;
        (void)gyro_id;
        (void)page_id_before;
        (void)page_id_after;
        (void)opr_mode_before;
        (void)opr_mode_after;
        (void)accel;
        (void)gyro;
        (void)sys_status;
        (void)sys_error;
        (void)self_test;
        for (volatile size_t i = 0; i < 100000; i++);
    }

    return 0;
}