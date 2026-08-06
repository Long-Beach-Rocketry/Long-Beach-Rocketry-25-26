/**
 * @file fake_i2c.h
 * @author Kent Hong
 * @brief Fake I2C class that serves as a communication 
 *        medium with simulated I2C sensors (inverse drivers).
 */
#pragma once
#include <cstdint>
#include <span>
#include "i2c.h"

namespace LBR
{
class FakeI2C : public I2c
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
    bool mem_read(std::span<uint8_t> data, const uint8_t reg_addr,
                  uint8_t dev_addr) override;

    /**
     * @brief Read data from external device that uses 16-bit memory addresses
     * 
     * @param data block of memory to read data into from the bus
     * @param reg_addr data register of external device to read from
     * @param dev_addr address of target device
     * @return true if successful, false otherwise
     */
    bool mem_read(std::span<uint8_t> data, const uint16_t reg_addr,
                  uint8_t dev_addr) override;

    /**
     * @brief Writes data to external device that uses 8-bit memory addresses
     * 
     * @param data block of memory storing data to write into the bus
     * @param reg_addr data register of external device to write to
     * @param dev_addr address of target device
     * @return true if successful, false otherwise
     */
    bool mem_write(std::span<const uint8_t> data, const uint8_t reg_addr,
                   uint8_t dev_addr) override;

    /**
     * @brief Writes data to external device that uses 16-bit memory addresses
     * 
     * @param data block of memory storing data to write into the bus
     * @param reg_addr data register of external device to write to
     * @param dev_addr address of target device
     * @return true if successful, false otherwise
     */
    bool mem_write(std::span<const uint8_t> data, const uint16_t reg_addr,
                   uint8_t dev_addr) override;

    /**
     * @brief Read raw data from an I2C bus
     * 
     * @param data block of memory to read data into from the bus
     * @param dev_addr address of target device
     * @return true if successful, false otherwise
     */
    bool read(std::span<uint8_t> data, uint8_t dev_addr) override;

    /**
     * @brief Write raw data to an I2C bus
     * 
     * @param data block of memory to write data into the bus
     * @param dev_addr address of target device
     * @return true if successful, false otherwise
     */
    bool write(std::span<const uint8_t> data, uint8_t dev_addr) override;

    ~FakeI2C() = default;
};
}  // namespace LBR
