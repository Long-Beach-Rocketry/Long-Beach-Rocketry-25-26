/**
 * @file inv_bmp390.h
 * @author Kent Hong
 * @brief Inverse Driver class that simulates a BMP390 barometer I2C sensor
 */
#pragma once
#include "inv_i2c_sensor.h"

namespace LBR
{

// Would be nice if I could use the values directly from bmp390.h/cc
// ^ Single source of truth
constexpr uint8_t Bmp390_CHIP_ID_REG = 0x00;
constexpr uint8_t Bmp390_PWR_CTRL_REG = 0x1B;

class InvBmp390 : public InvI2cSensor
{
public:
    explicit InvBmp390(uint8_t dev_addr);

    bool process_read(const uint8_t reg_addr, uint8_t& data) override;

    bool process_read(uint8_t& data) override;

    bool process_write(const uint8_t reg_addr, uint8_t data) override;

    bool process_write(uint8_t data) override;

    bool get_8bit_addr(uint8_t& addr) override;

private:
    static constexpr uint8_t CHIP_ID = 0x60U;
    uint8_t dev_addr_;
    uint8_t recent_reg_{Bno055_CHIP_ID_REG};
};
}  // namespace LBR