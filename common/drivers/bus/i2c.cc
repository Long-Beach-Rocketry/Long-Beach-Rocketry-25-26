/**
 * @file i2c.cc
 * @brief Generic implementations for I2C driver
 * @author Yshi Blanco
 * @date 11/24/2025
 */

#include "i2c.h"

namespace LBR
{
bool I2c::mem_read(std::span<uint8_t> data, const uint8_t reg_addr,
                   uint8_t dev_addr)
{
    // Creating span for register address
    const uint8_t reg[1] = {reg_addr};
    std::span<const uint8_t> register_addr{reg, 1};

    bool ret = true;

    // Sending register address to read from
    ret = ret && burst_write(register_addr, dev_addr);
    if (ret == false)
    {
        return ret;
    }

    // Data transfer
    ret = ret && burst_read(data, dev_addr);

    return ret;
}

bool I2c::mem_read(std::span<uint8_t> data, const uint16_t reg_addr,
                   uint8_t dev_addr)
{
    // Creating span for register address
    const uint8_t reg[2] = {static_cast<uint8_t>((reg_addr >> 8) & 0xFF),
                            static_cast<uint8_t>(reg_addr & 0xFF)};
    std::span<const uint8_t> register_addr{reg, 2};

    bool ret = true;

    // Sending register address to read from
    ret = ret && burst_write(register_addr, dev_addr);
    if (ret == false)
    {
        return ret;
    }

    // Data transfer
    ret = ret && burst_read(data, dev_addr);

    return ret;
}

bool I2c::mem_write(std::span<const uint8_t> data, const uint8_t reg_addr,
                    uint8_t dev_addr)
{
    // Creating span for register address
    const uint8_t reg[1] = {reg_addr};
    std::span<const uint8_t> register_addr{reg, 1};

    bool ret = true;

    // Sending register address to write to
    ret = ret && burst_write(register_addr, dev_addr);
    if (ret == false)
    {
        return ret;
    }

    // Data transfer
    ret = ret && burst_write(data, dev_addr);

    return ret;
}

bool I2c::mem_write(std::span<const uint8_t> data, const uint16_t reg_addr,
                    uint8_t dev_addr)
{
    // Creating span for register address
    const uint8_t reg[2] = {static_cast<uint8_t>((reg_addr >> 8) & 0xFF),
                            static_cast<uint8_t>(reg_addr & 0xFF)};
    std::span<const uint8_t> register_addr{reg, 2};

    bool ret = true;

    // Sending register address to write to
    ret = ret && burst_write(register_addr, dev_addr);
    if (ret == false)
    {
        return ret;
    }

    // Data transfer
    ret = ret && burst_write(data, dev_addr);

    return ret;
}
}  // namespace LBR