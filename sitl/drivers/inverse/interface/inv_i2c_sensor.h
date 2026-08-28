/**
 * @file inv_i2c_sensor.h
 * @author Kent Hong
 * @brief Inverse Driver Class for Abstract I2C Sensors
 */
#pragma once
#include <cstdint>
#include <iostream>
#include <span>

class InvI2cSensor
{
public:
    InvI2cSensor() = default;
    virtual ~InvI2cSensor() = default;

    /**
     * @brief Read handler for inverse sensors with 8-bit memory addresses that 
     * use the i2c bus.
     * 
     * @param reg_addr the specific register address of the inverse driver device
     * @param data the buffer that will be written into from the device registers
     * @return true if the operation was successful, otherwise false
     */
    virtual bool process_read(const uint8_t reg_addr, uint8_t& data)
    {
        return_error_8bit();
    }

    /**
     * @brief Read handler for inverse sensors with 16-bit memory addresses that 
     * use the i2c bus.
     * 
     * @param reg_addr the specific register address of the inverse driver device
     * @param data the buffer that will be written into from the device registers
     * @return true if the operation was successful, otherwise false
     */
    virtual bool process_read(const uint16_t reg_addr, uint8_t& data)
    {
        return return_error_16bit();
    }

    /**
     * @brief Read handler for inverse sensors that 
     * use the i2c bus. It will read the most recently used register.
     * 
     * @param data the buffer that will be written into from the device registers
     * @return true if the operation was successful, otherwise false
     */
    virtual bool process_read(uint8_t& data) = 0;

    /**
     * @brief Write handler for inverse sensors with 8-bit memory addresses that 
     * use the i2c bus.
     * 
     * @param reg_addr the specific register address of the inverse driver device
     * @param data the data that will be written the to device registers
     * @return false by default assuming no 8-bit address, otherwise true if successful
     */
    virtual bool process_write(const uint8_t reg_addr, uint8_t data)
    {
        return_error_8bit();
    }

    /**
     * @brief Write handler for inverse sensors with 16-bit memory addresses that 
     * use the i2c bus.
     * 
     * @param reg_addr the specific register address of the inverse driver device
     * @param data the data that will be written the to device registers
     * @return false assuming no 16-bit address, otherwise true if successful
     */
    virtual bool process_write(const uint16_t reg_addr, uint8_t data)
    {
        return return_error_16bit();
    }

    /**
     * @brief Write handler for inverse sensors that 
     * use the i2c bus. It write to the the most recently used register.
     * 
     * @param data the data that will be written the to device registers
     * @return false assuming no 8-bit address, otherwise true if successful
     */
    virtual bool process_write(const uint8_t data) = 0;

    /**
     * @brief Simple address getters.
     * 
     * @param addr to be set as the sensor address for the i2c bus
     * @return true if the device has a valid address, false otherwise
     */
    virtual bool get_8bit_addr(uint8_t& addr)
    {
        return_error_8bit();
    }

    /**
     * @brief Simple reusable error returning method.
     * 
     * @return false 
     */
    bool return_error_8bit(void)
    {
        std::cout << "This device does not have a 8-bit address.\n";
        return false;
    }

    /**
     * @brief Simple reusable error returning method. 
     * 
     * @return false
     */
    bool return_error_16bit(void)
    {
        std::cout << "This device does not have a 16-bit address.\n";
        return false;
    }
};