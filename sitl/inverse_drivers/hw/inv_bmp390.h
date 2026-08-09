/**
 * @file inv_bmp390.h
 * @author Kent Hong
 * @brief Inverse Driver class that simulates a BMP390 barometer I2C sensor
 */
#pragma once
#include "inv_i2c_sensor.h"

namespace LBR
{

class InvBmp390 : public InvI2cSensor
{
public:
    InvBmp390() = default;
    ~InvBmp390() = default;

    bool process_read(const uint8_t reg_addr, uint8_t& data) override;

    bool process_write(const uint8_t reg_addr, uint8_t data) override;

    bool get_8bit_addr(uint8_t& addr) override;

private:
    static constexpr uint8_t CHIP_ID = 0x60U;
};
}  // namespace LBR