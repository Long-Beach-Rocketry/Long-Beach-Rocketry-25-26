/**
 * @file st_i2c.cc
 * @brief I2C driver implementation for STM32L476xx
 * @author Yshi Blanco
 * @date 10/02/2025
 */

#include "st_i2c.h"

namespace LBR
{
namespace Stml4
{
HwI2c::HwI2c(const StI2cParams& params)
    : _base_addr{params.base_addr}, _timingr{params.timingr}
{
}

bool HwI2c::init()
{
    if (_base_addr == nullptr)
    {
        return false;
    }

    // Reset peripheral
    _base_addr->CR1 &= ~I2C_CR1_PE;

    // Configure timing
    _base_addr->TIMINGR = _timingr;

    _base_addr->CR2 &= ~I2C_CR2_ADD10;  // 7-bit addressing mode

    // Enable peripheral
    _base_addr->CR1 |= I2C_CR1_PE;

    return true;
}

bool HwI2c::burst_read(std::span<uint8_t> data, uint8_t dev_addr)
{

    if (_base_addr == nullptr)
    {
        return false;
    }

    // Check if init was called
    if (!(_base_addr->CR1 & I2C_CR1_PE))
    {
        return false;
    }

    /**
     * Setting target
     * Checking if communication in progress
     */
    if (_base_addr->CR2 & I2C_CR2_START)
    {
        return false;
    }

    _base_addr->CR2 &= ~I2C_CR2_SADD;  // Clearing address
    _base_addr->CR2 |= (dev_addr << (I2C_CR2_SADD_Pos + 1));  // Setting address

    // Make sure I2C bus is idle
    if (_base_addr->ISR & I2C_ISR_BUSY)
    {
        return false;
    }

    // Configure for reading
    _base_addr->CR2 &= ~I2C_CR2_NBYTES;
    _base_addr->CR2 |= ((data.size() << I2C_CR2_NBYTES_Pos) | I2C_CR2_RD_WRN |
                        I2C_CR2_AUTOEND);

    // Initiate read
    _base_addr->CR2 |= I2C_CR2_START;

    for (uint8_t& byte : data)
    {
        // Wait for transfer with timeout
        uint32_t rx_timeout = 100000;
        while (!(_base_addr->ISR & I2C_ISR_RXNE) && --rx_timeout)
        {
        }
        if (rx_timeout == 0)
        {
            // Timeout occurred
            return false;
        }
        byte = _base_addr->RXDR;
    }

    // Detect stop with timeout
    uint32_t stop_timeout = 100000;
    while (!(_base_addr->ISR & I2C_ISR_STOPF) && --stop_timeout)
    {
    }
    if (stop_timeout == 0)
    {
        // Timeout occurred
        return false;
    }
    _base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
}

bool HwI2c::burst_write(std::span<const uint8_t> data, uint8_t dev_addr)
{
    if (_base_addr == nullptr)
    {
        return false;
    }

    // Check if init was called
    if (!(_base_addr->CR1 & I2C_CR1_PE))
    {
        return false;
    }

    /**
     * Setting target
     * Check if communication is in progress
     */
    if (_base_addr->CR2 & I2C_CR2_START)
    {
        return false;
    }

    _base_addr->CR2 &= ~I2C_CR2_SADD;  // Clearing address
    _base_addr->CR2 |= (dev_addr << (I2C_CR2_SADD_Pos + 1));  // Setting address

    // Make sure I2C bus is idle
    if (_base_addr->ISR & I2C_ISR_BUSY)
    {
        return false;
    }

    // Configure for writing
    _base_addr->CR2 &= ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN);
    _base_addr->CR2 |= ((data.size() << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND);

    // Initiate write
    _base_addr->CR2 |= I2C_CR2_START;

    // Write
    for (const uint8_t byte : data)
    {
        // Wait for transfer or NACK with timeout
        uint32_t timeout =
            100000;  // I added timeout to prevent infinite blocking
        while (!(_base_addr->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF)) && --timeout)
        {
            if (_base_addr->ISR & I2C_ISR_NACKF)
            {
                _base_addr->ICR |= I2C_ICR_NACKCF;
                return false;
            }
        }
        if (timeout == 0)
        {
            // Timeout occurred
            return false;
        }
        _base_addr->TXDR = byte;
    }

    // Detect stop with timeout
    uint32_t stop_timeout = 100000;
    while (!(_base_addr->ISR & I2C_ISR_STOPF) && --stop_timeout)
    {
    }
    if (stop_timeout == 0)
    {
        // Timeout occurred
        return false;
    }
    _base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
}

}  // namespace Stml4
}  // namespace LBR