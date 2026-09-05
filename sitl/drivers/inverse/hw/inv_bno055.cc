#include "inv_bno055.h"

namespace LBR
{

static bool is_read_only(uint8_t reg_addr)
{
    if ((0x08 <= reg_addr && reg_addr <= 0x37) || reg_addr == 0x39 ||
        reg_addr == 0x3A)
    {
        return true;
    }
    return false;
}

InvBno055::InvBno055(uint8_t dev_addr)
    : dev_addr_{dev_addr}, recent_reg_{Bno055_CHIP_ID_REG}
{
}

bool InvBno055::init()
{
    register_map_ = {
        {0x00, 0xA0},  // CHIP ID
        {0x07, 0x00},  // PAGE_ID

        {0x08, 0x00},  // ACCEL_DATA_X_LSB
        {0x09, 0x00},  // ACCEL_DATA_X_MSB
        {0x0A, 0x00},  // ACCEL_DATA_Y_LSB
        {0x0B, 0x00},  // ACCEL_DATA_Y_MSB
        {0x0C, 0x00},  // ACCEL_DATA_Z_LS
        {0x0D, 0x00},  // ACCEL_DATA_Z_MS

        {0x0E, 0x00},  // MAG_DATA_X_LSB
        {0x0F, 0x00},  // MAG_DATA_X_MSB
        {0x10, 0x00},  // MAG_DATA_Y_LSB
        {0x11, 0x00},  // MAG_DATA_Y_MSB
        {0x12, 0x00},  // MAG_DATA_Z_LSB
        {0x13, 0x00},  // MAG_DATA_Z_MSB

        {0x14, 0x00},  // GYRO_DATA_X_LSB
        {0x15, 0x00},  // GYRO_DATA_X_MSB
        {0x16, 0x00},  // GYRO_DATA_Y_LSB
        {0x17, 0x00},  // GYRO_DATA_Y_MSB
        {0x18, 0x00},  // GYRO_DATA_Z_LSB
        {0x19, 0x00},  // GYRO_DATA_Z_MSB

        {0x1A, 0x00},  // EULER_H_LSB
        {0x1B, 0x00},  // EULER_H_MSB
        {0x1C, 0x00},  // EULER_R_LSB
        {0x1D, 0x00},  // EULER_R_MSB
        {0x1E, 0x00},  // EULER_P_LSB
        {0x1F, 0x00},  // EULER_P_MSB

        {0x20, 0x00},  // QUATERNION_DATA_W_LSB
        {0x21, 0x00},  // QUATERNION_DATA_W_MSB
        {0x22, 0x00},  // QUATERNION_DATA_X_LSB
        {0x23, 0x00},  // QUATERNION_DATA_X_MSB
        {0x24, 0x00},  // QUATERNION_DATA_Y_LSB
        {0x25, 0x00},  // QUATERNION_DATA_Y_MSB
        {0x26, 0x00},  // QUATERNION_DATA_Z_LSB
        {0x27, 0x00},  // QUATERNION_DATA_Z_MSB

        {0x28, 0x00},  // LINEAR_ACCEL_DATA_X_LSB
        {0x29, 0x00},  // LINEAR_ACCEL_DATA_X_MSB
        {0x2A, 0x00},  // LINEAR_ACCEL_DATA_Y_LSB
        {0x2B, 0x00},  // LINEAR_ACCEL_DATA_Y_MSB
        {0x2C, 0x00},  // LINEAR_ACCEL_DATA_Z_LSB
        {0x2D, 0x00},  // LINEAR_ACCEL_DATA_Z_MSB

        {0x2E, 0x00},  // GRAVITY_DATA_X_LSB
        {0x2F, 0x00},  // GRAVITY_DATA_X_MSB
        {0x30, 0x00},  // GRAVITY_DATA_Y_LSB
        {0x31, 0x00},  // GRAVITY_DATA_Y_MSB
        {0x32, 0x00},  // GRAVITY_DATA_Z_LSB
        {0x33, 0x00},  // GRAVITY_DATA_Z_MSB

        {0x35, 0x00},  // CALIB_STAT
        {0x36, 0x00},  // SELFTEST_RESULT
        {0x39, 0x00},  // SYS_STAT
        {0x3A, 0x00},  // SYS_ERR
        {0x3D, 0x00},  // OPR_MODE
        {0x3E, 0x00},  // PWR_MODE
        {0x3F, 0x00},  // SYS_TRIGGER
    };
}

bool InvBno055::process_read(const uint8_t reg_addr, uint8_t& data)
{
    recent_reg_ = reg_addr;

    auto it = register_map_.find(reg_addr);

    if (it == register_map_.end())
    {
        /* It silently fails */
        data = Bn055_NULL_BYTE;
        return true;
    }

    data = it->second;
    return true;
}

bool InvBno055::process_read(uint8_t& data)
{
    return process_read(recent_reg_, data);
}

bool InvBno055::process_write(const uint8_t reg_addr, uint8_t data)
{

    recent_reg_ = reg_addr;
    auto it = register_map_.find(reg_addr);

    if (it != register_map_.end() && !is_read_only(reg_addr))
    {
        register_map_[reg_addr] = data;
    }
    else
    { /* It silently fails */
    }
    return true;
}

bool InvBno055::process_write(uint8_t data)
{
    return process_write(recent_reg_, data);
}

bool InvBno055::get_8bit_addr(uint8_t& addr)
{
    addr = dev_addr_;
    return true;
}

}  // namespace LBR