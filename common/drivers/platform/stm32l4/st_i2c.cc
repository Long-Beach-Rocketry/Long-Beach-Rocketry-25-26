/**
 * @file st_i2c.cc
 * @brief I2C driver implementation for STM32L476xx
 * @author Yshi Blanco
 * @date 10/02/2025
 */

#include "st_i2c.h"
#include <array>

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

bool HwI2c::mem_read(std::span<uint8_t> data, const uint8_t reg_addr,
                     uint8_t dev_addr)
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

    // Make sure I2C bus is idle
    if (_base_addr->ISR & I2C_ISR_BUSY)
    {
        return false;
    }

    // Writing register address to write to
    _base_addr->CR2 &=
        ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_SADD | I2C_CR2_AUTOEND);
    _base_addr->CR2 |= (1 << I2C_CR2_NBYTES_Pos |
                        (dev_addr << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_START);

    while (!(_base_addr->ISR & I2C_ISR_TXIS))
    {
        // TODO: replace while with timeout
    }

    _base_addr->TXDR = reg_addr;

    while (!(_base_addr->ISR & I2C_ISR_TC))
    {
        // TODO : replace while with timeout
    }

    // Configuring and initiating transfer
    _base_addr->CR2 &=
        ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_SADD | I2C_CR2_AUTOEND);
    _base_addr->CR2 |= ((data.size() << I2C_CR2_NBYTES_Pos) | I2C_CR2_RD_WRN |
                        (dev_addr << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_AUTOEND |
                        I2C_CR2_START);

    for (uint8_t& byte : data)
    {
        // Wait for transfer
        while (!(_base_addr->ISR & I2C_ISR_RXNE))
        {
            // TODO: replace while with timeout
        }

        byte = _base_addr->RXDR;
    }

    // Detect stop
    while (!(_base_addr->ISR & I2C_ISR_STOPF))
    {
        // TODO: replace while with timeout
    }
    _base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
}

bool HwI2c::mem_read(std::span<uint8_t> data, const uint16_t reg_addr,
                     uint8_t dev_addr)
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

    // Make sure I2C bus is idle
    if (_base_addr->ISR & I2C_ISR_BUSY)
    {
        return false;
    }

    // Writing register address to write to
    _base_addr->CR2 &=
        ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_SADD | I2C_CR2_AUTOEND);
    _base_addr->CR2 |= (2 << I2C_CR2_NBYTES_Pos |
                        (dev_addr << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_START);

    while (!(_base_addr->ISR & I2C_ISR_TXIS))
    {
        // TODO: replace while with timeout
    }
    _base_addr->TXDR = static_cast<uint8_t>((reg_addr >> 8) & 0xFF);  // MSB
    while (!(_base_addr->ISR & I2C_ISR_TXIS))
    {
        // TODO: replace while with timeout
    }
    _base_addr->TXDR = static_cast<uint8_t>(reg_addr & 0xFF);  // LSB

    while (!(_base_addr->ISR & I2C_ISR_TC))
    {
        // TODO : replace while with timeout
    }

    // Configuring and initiating transfer
    _base_addr->CR2 &=
        ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_SADD | I2C_CR2_AUTOEND);
    _base_addr->CR2 |= ((data.size() << I2C_CR2_NBYTES_Pos) | I2C_CR2_RD_WRN |
                        (dev_addr << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_AUTOEND |
                        I2C_CR2_START);

    for (uint8_t& byte : data)
    {
        // Wait for transfer
        while (!(_base_addr->ISR & I2C_ISR_RXNE))
        {
            // TODO: replace while with timeout
        }

        byte = _base_addr->RXDR;
    }

    // Detect stop
    while (!(_base_addr->ISR & I2C_ISR_STOPF))
    {
        // TODO: replace while with timeout
    }
    _base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
}

bool HwI2c::mem_write(std::span<const uint8_t> data, const uint8_t reg_addr,
                      uint8_t dev_addr)
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

    // Make sure I2C bus is idle
    if (_base_addr->ISR & I2C_ISR_BUSY)
    {
        return false;
    }

    // Configuring and initiating transfer
    _base_addr->CR2 &=
        ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_SADD | I2C_CR2_AUTOEND);
    _base_addr->CR2 |= (((data.size() + 1) << I2C_CR2_NBYTES_Pos) |
                        (dev_addr << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_AUTOEND |
                        I2C_CR2_START);

    // Write register address to write to
    while (!(_base_addr->ISR & I2C_ISR_TXIS))
    {
        // TODO: replace while with timeout
    }
    _base_addr->TXDR = reg_addr;

    // Writing data
    for (const uint8_t byte : data)
    {
        // Wait for transfer or NACK
        while (!(_base_addr->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF)))
        {
            // TODO: replace while with timeout
        }

        if (_base_addr->ISR & I2C_ISR_NACKF)
        {
            _base_addr->ICR |= I2C_ICR_NACKCF;
            return false;
        }

        _base_addr->TXDR = byte;
    }

    // Detect stop
    while (!(_base_addr->ISR & I2C_ISR_STOPF))
    {
        // TODO: replace while with timeout
    }
    _base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
}

bool HwI2c::mem_write(std::span<const uint8_t> data, const uint16_t reg_addr,
                      uint8_t dev_addr)
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

    // Make sure I2C bus is idle
    if (_base_addr->ISR & I2C_ISR_BUSY)
    {
        return false;
    }

    // Configuring and initiating transfer
    _base_addr->CR2 &=
        ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_SADD | I2C_CR2_AUTOEND);
    _base_addr->CR2 |= (((data.size() + 2) << I2C_CR2_NBYTES_Pos) |
                        (dev_addr << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_AUTOEND |
                        I2C_CR2_START);

    // Write register address to write to
    while (!(_base_addr->ISR & I2C_ISR_TXIS))
    {
        // TODO: replace while with timeout
    }
    _base_addr->TXDR = static_cast<uint8_t>((reg_addr >> 8) & 0xFF);  // MSB
    while (!(_base_addr->ISR & I2C_ISR_TXIS))
    {
        // TODO: replace while with timeout
    }
    _base_addr->TXDR = static_cast<uint8_t>(reg_addr & 0xFF);  // LSB

    // Writing data
    for (const uint8_t byte : data)
    {
        // Wait for transfer or NACK
        while (!(_base_addr->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF)))
        {
            // TODO: replace while with timeout
        }

        if (_base_addr->ISR & I2C_ISR_NACKF)
        {
            _base_addr->ICR |= I2C_ICR_NACKCF;
            return false;
        }

        _base_addr->TXDR = byte;
    }

    // Detect stop
    while (!(_base_addr->ISR & I2C_ISR_STOPF))
    {
        // TODO: replace while with timeout
    }
    _base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
}

bool HwI2c::read(std::span<uint8_t> data, uint8_t dev_addr)
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

    // Make sure I2C bus is idle
    if (_base_addr->ISR & I2C_ISR_BUSY)
    {
        return false;
    }

    // Configuring and initiating transfer
    _base_addr->CR2 &=
        ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_SADD | I2C_CR2_AUTOEND);
    _base_addr->CR2 |= ((data.size() << I2C_CR2_NBYTES_Pos) | I2C_CR2_RD_WRN |
                        (dev_addr << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_AUTOEND |
                        I2C_CR2_START);

    for (uint8_t& byte : data)
    {
        // Wait for transfer
        while (!(_base_addr->ISR & I2C_ISR_RXNE))
        {
            // TODO: replace while with timeout
        }

        byte = _base_addr->RXDR;
    }

    // Detect stop
    while (!(_base_addr->ISR & I2C_ISR_STOPF))
    {
        // TODO: replace while with timeout
    }
    _base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
}

bool HwI2c::write(std::span<const uint8_t> data, uint8_t dev_addr)
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

    // Make sure I2C bus is idle
    if (_base_addr->ISR & I2C_ISR_BUSY)
    {
        return false;
    }

    // Configuring and initiating transfer
    _base_addr->CR2 &=
        ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_SADD | I2C_CR2_AUTOEND);
    _base_addr->CR2 |= ((data.size() << I2C_CR2_NBYTES_Pos) |
                        (dev_addr << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_AUTOEND |
                        I2C_CR2_START);

    // Write
    for (const uint8_t byte : data)
    {
        // Wait for transfer or NACK
        while (!(_base_addr->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF)))
        {
            // TODO: replace while with timeout
        }

        if (_base_addr->ISR & I2C_ISR_NACKF)
        {
            _base_addr->ICR |= I2C_ICR_NACKCF;
            return false;
        }

        _base_addr->TXDR = byte;
    }

    // Detect stop
    while (!(_base_addr->ISR & I2C_ISR_STOPF))
    {
        // TODO: replace while with timeout
    }
    _base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
}

}  // namespace Stml4
}  // namespace LBR
