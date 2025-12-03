/**
 * @file bno055_imu.cc
 * @brief BNO055 IMU sensor implementation
 * @author Bex
 * @date 2025-10-22
 */

#include "bno055_imu.h"
#include "delay.h"
#include "st_i2c.h"  
#include <span>

namespace LBR {

using namespace LBR::Stml4;
using LBR::Utils::DelayMs;

Bno055::Bno055(LBR::Stml4::HwI2c& i2c, uint8_t addr, I2cMemReadFn read_fn, I2cMemWriteFn write_fn)
    : i2c_(i2c), address_(addr), mem_read_fn_(read_fn), mem_write_fn_(write_fn) {}

//low level I2C communication helpers

uint8_t Bno055::read_reg(uint8_t reg)
{
    uint8_t value = 0;
    
    // Use repeated start callback (if provided)
    if (mem_read_fn_) {
        mem_read_fn_(address_, reg, &value, 1);
    }

    return value;
}

void Bno055::write_reg(uint8_t reg, uint8_t value)
{
    if (mem_write_fn_) {
        mem_write_fn_(address_, reg, &value, 1);
    }
    // Note: No fallback - write callback required
}

void Bno055::read_multi(uint8_t start, uint8_t* buf, size_t len)
{
    // Use repeated start callback (required for bare-metal safety)
    if (mem_read_fn_) {
        mem_read_fn_(address_, start, buf, len);
    }
    // Note: No fallback - repeated start is required for BNO055 to work correctly
}

void Bno055::set_mode(Mode mode)
{
    write_reg(SysReg::OPR_MODE, static_cast<uint8_t>(mode));
    DelayMs(30);
}


/* Initialization */

void Bno055::init()
{
    DelayMs(650);

    uint8_t id = read_reg(SysReg::CHIP_ID);
    for (int i = 0; i < 5 && id != 0xA0; i++) {
        DelayMs(10);
        id = read_reg(SysReg::CHIP_ID);
    }
    if (id != 0xA0) return;

    set_mode(Mode::CONFIG);
    DelayMs(25);

    write_reg(SysReg::PWR_MODE, 0x00);
    write_reg(SysReg::PAGE_ID,  0x00);

    set_mode(Mode::IMU);
    DelayMs(20);
}

/* Deinit */
void Bno055::deinit()
{
    // Put device into deep suspend to save power
    set_mode(Mode::CONFIG);
    DelayMs(10);
    write_reg(SysReg::PWR_MODE, PowerReg::SUSPEND);
    DelayMs(25);
}

/* Sensor Data Reads */

// Helpers for combining LSB and MSB
static inline int16_t combine(uint8_t lsb, uint8_t msb) 
{
    return (static_cast<int16_t>(msb) << 8) | lsb;
}

bool Bno055::read_all(Bno055Data& out)
{
    uint8_t buf[6 + 6 + 6 + 6 + 8];   // ACC + GYR + LIA + GRAV + QUAT
    size_t idx = 0;

    // Read everything starting from ACC register
    read_multi(RegStart::ACC, buf, sizeof(buf));

    // Parse accel
    out.accel.x = combine(buf[0], buf[1]) / 100.0f;
    out.accel.y = combine(buf[2], buf[3]) / 100.0f;
    out.accel.z = combine(buf[4], buf[5]) / 100.0f;
    idx += 6;

    // Parse gyro
    out.gyro.x = combine(buf[idx+0], buf[idx+1]) / 16.0f;
    out.gyro.y = combine(buf[idx+2], buf[idx+3]) / 16.0f;
    out.gyro.z = combine(buf[idx+4], buf[idx+5]) / 16.0f;
    idx += 6;

    // Parse linear accel
    out.linear_accel.x = combine(buf[idx+0], buf[idx+1]) / 100.0f;
    out.linear_accel.y = combine(buf[idx+2], buf[idx+3]) / 100.0f;
    out.linear_accel.z = combine(buf[idx+4], buf[idx+5]) / 100.0f;
    idx += 6;

    // Parse gravity
    out.gravity.x = combine(buf[idx+0], buf[idx+1]) / 100.0f;
    out.gravity.y = combine(buf[idx+2], buf[idx+3]) / 100.0f;
    out.gravity.z = combine(buf[idx+4], buf[idx+5]) / 100.0f;
    idx += 6;

    // Parse quaternion
    out.quat.w = combine(buf[idx+0], buf[idx+1]) / 16384.0f;
    out.quat.x = combine(buf[idx+2], buf[idx+3]) / 16384.0f;
    out.quat.y = combine(buf[idx+4], buf[idx+5]) / 16384.0f;
    out.quat.z = combine(buf[idx+6], buf[idx+7]) / 16384.0f;

    return true;
}



/* Status & Diagnostics */

uint8_t Bno055::calibrate()
{
    return read_reg(SysReg::CALIB_STAT);
}

uint8_t Bno055::get_sys_status()
{
    return read_reg(SysReg::SYS_STATUS);
}

uint8_t Bno055::get_sys_error()
{
    return read_reg(SysReg::SYS_ERR);
}

/* Built-in Self Tests */

uint8_t Bno055::run_post()
{
    set_mode(Mode::CONFIG);
    DelayMs(25);

    write_reg(SysReg::SYS_TRIGGER, 0x80);
    DelayMs(650);

    uint8_t status = read_reg(SysReg::ST_RESULT);

    set_mode(Mode::IMU);
    DelayMs(20);

    return status;
}

uint8_t Bno055::run_bist()
{
    set_mode(Mode::CONFIG);
    DelayMs(25);

    write_reg(SysReg::SYS_TRIGGER, 0x01);
    DelayMs(650);

    uint8_t status = read_reg(SysReg::ST_RESULT);

    set_mode(Mode::IMU);
    DelayMs(20);

    return status;
}


// Testing Purpose 

uint8_t Bno055::get_chip_id(uint8_t& id)
{
    return read_reg(SysReg::CHIP_ID);
}

} // namespace LBR