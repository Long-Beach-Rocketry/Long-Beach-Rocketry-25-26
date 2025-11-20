/**
 * @file i2c.h
 * @brief I2C driver interface
 * @author Yshi Blanco
 * @date 10/02/2025
 */

#pragma once
#include <cstdint>
#include <span>

namespace LBR
{
/**
 * @class I2c
 * @brief I2c driver instance
 */
class I2c
{
public:
    /**
     * @brief Read data from external device
     * 
     * @param data block of memory to read data into from the bus
     * @param reg_addr data register of external device to read from
     * @param dev_addr address of target device
     * @return true if successful, false otherwise
     */
    virtual bool mem_read(std::span<uint8_t> data,
                          std::span<const uint8_t> reg_addr,
                          uint8_t dev_addr) = 0;

    /**
     * @brief Writes data to external device
     * 
     * @param data block of memory storing data to write into the bus
     * @param reg_addr data register of external device to write to
     * @param dev_addr address of target device
     * @return true if successful, false otherwise
     */
    virtual bool mem_write(std::span<const uint8_t> data,
                           std::span<const uint8_t> reg_addr,
                           uint8_t dev_addr) = 0;

    ~I2c() = default;
};
}  // namespace LBR