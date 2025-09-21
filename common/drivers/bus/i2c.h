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
     * @param dev_addr address of target device
     * @return true if successful, false otherwise
     */
    virtual bool mem_read(std::span<uint8_t> data, uint8_t dev_addr) = 0;

    /**
     * @brief Writes data to external device
     * 
     * @param data block of memory storing data to write into the bus
     * @param dev_addr address of target device
     * @return true if successful, false otherwise
     */
    virtual bool mem_write(std::span<const uint8_t> data, uint8_t dev_addr) = 0;

    ~I2c() = default;
};
}  // namespace LBR
