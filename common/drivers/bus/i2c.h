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
     * @brief Read data from external device that uses 8-bit memory addresses
     * 
     * @param data block of memory to read data into from the bus
     * @param reg_addr data register of external device to read from
     * @param dev_addr address of target device
     * @return true if successful, false otherwise
     */
    virtual bool mem_read(std::span<uint8_t> data, const uint8_t reg_addr,
                          uint8_t dev_addr) = 0;

    /**
     * @brief Read data from external device that uses 16-bit memory addresses
     * 
     * @param data block of memory to read data into from the bus
     * @param reg_addr data register of external device to read from
     * @param dev_addr address of target device
     * @return true if successful, false otherwise
     */
    virtual bool mem_read(std::span<uint8_t> data, const uint16_t reg_addr,
                          uint8_t dev_addr) = 0;

    /**
     * @brief Writes data to external device that uses 8-bit memory addresses
     * 
     * @param data block of memory storing data to write into the bus
     * @param reg_addr data register of external device to write to
     * @param dev_addr address of target device
     * @return true if successful, false otherwise
     */
    virtual bool mem_write(std::span<const uint8_t> data,
                           const uint8_t reg_addr, uint8_t dev_addr) = 0;

    /**
     * @brief Writes data to external device that uses 16-bit memory addresses
     * 
     * @param data block of memory storing data to write into the bus
     * @param reg_addr data register of external device to write to
     * @param dev_addr address of target device
     * @return true if successful, false otherwise
     */
    virtual bool mem_write(std::span<const uint8_t> data,
                           const uint16_t reg_addr, uint8_t dev_addr) = 0;

    ~I2c() = default;

private:
    /**
     * @brief Initiate read/write
     * 
     * @param dev_addr address of target device
     * @param mode start/stop generation mode
     * @param dir direction of transfer
     * @param num_bytes number of bytes being transferred
     * @return true if successful, false otherwise
     */
    virtual bool initiate_transfer(uint8_t dev_addr, uint8_t mode, uint8_t dir,
                                   size_t num_bytes) = 0;

    /**
     * @brief Detects stop condition and clears flag
     * 
     * @return true if successful, false otherwise
     */
    virtual bool detect_stop() = 0;

    /**
     * @brief Read raw data from an I2C bus
     * 
     * @param data block of memory to read data into from the bus
     * @return true if successful, false otherwise
     * 
     * @note This transfer does not create a start/stop condition
     */
    virtual bool burst_read(std::span<uint8_t> data) = 0;

    /**
     * @brief Write raw data to an I2C bus
     * 
     * @param data block of memory to write data into the bus
     * @return true if successful, false otherwise
     * 
     * @note This transfer does not create a start/stop condition
     */
    virtual bool burst_write(std::span<const uint8_t> data) = 0;
};
}  // namespace LBR