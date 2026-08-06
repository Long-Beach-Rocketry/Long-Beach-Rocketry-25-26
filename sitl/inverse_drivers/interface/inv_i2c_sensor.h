/**
 * @file inv_i2c_sensor.h
 * @author Kent Hong
 * @brief Inverse Driver Class for Abstract I2C Sensors
 */
#pragma once
#include <cstdint>
#include <iostream>
#include <span>

class InvI2CSensor
{
public:
    InvI2CSensor() = default;
    virtual ~InvI2CSensor() = default;

    // TODO: Figure out opcode interface for the inverse drivers
    virtual bool process_read(uint8_t opcode, const uint8_t reg_addr,
                              uint8_t dev_addr) = 0;
    virtual bool get_8bit_addr(uint8_t& addr)
    {
        std::cout << "This device does not have a 8-bit address.\n";
        return false;
    }
    virtual bool get_16bit_addr(uint16_t& addr)
    {
        std::cout << "This device does not have a 16-bit address.\n";
        return false;
    }
};