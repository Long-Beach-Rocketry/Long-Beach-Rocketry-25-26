#pragma once
#include "inv_i2c_sensor.h"

namespace LBR
{

// It'd be nice if I could use the values directly from bno055_imu.h/cc
// ^ Single source of truth

constexpr uint8_t Bno055_CHIP_ID_REG = 0x00U;
constexpr uint8_t Bn055_NULL_BYTE = 0x00U;

class InvBno055 : public InvI2cSensor
{
public:
    explicit InvBno055(uint8_t dev_addr);

    bool process_read(const uint8_t reg_addr, uint8_t& data) override;

    bool process_read(uint8_t& data) override;

    bool process_write(const uint8_t reg_addr, uint8_t data) override;

    bool process_write(uint8_t data) override;

    bool get_8bit_addr(uint8_t& addr) override;

private:
    static constexpr uint8_t CHIP_ID = 0xA0U;
    uint8_t dev_addr_;
    uint8_t recent_reg_{Bno055_CHIP_ID_REG};
};

}  // namespace LBR