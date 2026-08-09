/**
 * @file fake_I2c.h
 * @author Kent Hong
 * @brief Fake I2c class that serves as a communication 
 *        medium with simulated I2c sensors (inverse drivers).
 */
#pragma once
#include <cstdint>
#include <span>
#include <unordered_map>
#include "I2c.h"
#include "inv_I2c_sensor.h"

namespace LBR
{
class FakeI2c : public I2c
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
     * @brief Read raw data from an I2c bus
     * 
     * @param data block of memory to read data into from the bus
     * @param dev_addr address of target device
     * @return true if successful, false otherwise
     */
    bool read(std::span<uint8_t> data, uint8_t dev_addr) override;

    /**
     * @brief Write raw data to an I2c bus
     * 
     * @param data block of memory to write data into the bus
     * @param dev_addr address of target device
     * @return true if successful, false otherwise
     */
    bool write(std::span<const uint8_t> data, uint8_t dev_addr) override;

    ~FakeI2c() = default;

    /**
     * @note The following are SITL specific methods.
     */

    /**
     * @brief Adds the device address and devices to a lookup table to find the 
     * sensor later.
     * 
     * @param dev_addr device address the I2c bus uses to find the device
     * @param device the corresponding device
     * @return true if the address does not exist, false otherwise
     */
    bool register_device(uint8_t dev_addr, InvI2cSensor* device);

    /**
     * @brief Adds the device address and devices to a lookup table to find the 
     * sensor later.
     * 
     * @param dev_addr device address the I2c bus uses to find the device
     * @param device the corresponding device
     * @return true if the address does not exist, false otherwise
     */
    bool register_device(uint16_t dev_addr, InvI2cSensor* device);

    /**
     * @brief Finds the matching device given the device's address
     * 
     * @param dev_addr device address the I2c bus uses to find the device
     * @return InvI2cSensor pointer if it exists, otherwise nullptr
     */
    InvI2cSensor* find_device(uint8_t dev_addr) const;

    /**
     * @brief Finds the matching device given the device's address
     * 
     * @param dev_addr device address the I2c bus uses to find the device
     * @return InvI2cSensor pointer if it exists, otherwise nullptr
     */
    InvI2cSensor* find_device(uint16_t dev_addr) const;

private:
    /**
     * @brief Look-up tables to find a sensor using the i2c bus given its address.
     */
    std::unordered_map<uint8_t, InvI2cSensor*> devices_8addr;
    std::unordered_map<uint8_t, InvI2cSensor*> devices_16addr;
};
}  // namespace LBR
