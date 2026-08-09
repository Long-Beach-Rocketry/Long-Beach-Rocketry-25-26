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
     * @brief Read methods for sensors that use the i2c bus.
     */
    // TODO: Figure out opcode interface for the inverse drivers
    virtual bool process_read(const uint8_t reg_addr, uint8_t& data)
    {
        return_error_8bit();
    }
    virtual bool process_read(const uint16_t reg_addr, uint8_t& data)
    {
        return return_error_16bit();
    }
    virtual bool process_read(uint8_t& data) = 0;

    /**
     * @brief Write methods for sensors that use the i2c bus.
     */

    virtual bool process_write(const uint8_t reg_addr, uint8_t data)
    {
        return_error_8bit();
    }
    virtual bool process_write(const uint16_t reg_addr, uint8_t data)
    {
        return return_error_16bit();
    }
    virtual bool process_write(const uint8_t data) = 0;

    /**
     * @brief Simple address getters.
     * 
     * @param addr to be set as the sensor address for the i2c bus
     * @return true if it has a valid address, false otherwise
     */

    virtual bool get_8bit_addr(uint8_t& addr)
    {
        return_error_8bit();
    }
    virtual bool get_16bit_addr(uint16_t& addr)
    {
        return return_error_16bit();
    }

    /**
     * @brief Simple reusable error returning methods. 
     */
    bool return_error_8bit(void)
    {
        std::cout << "This device does not have a 8-bit address.\n";
        return false;
    }

    bool return_error_16bit(void)
    {
        std::cout << "This device does not have a 16-bit address.\n";
        return false;
    }
};