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

bool HwI2c::read(std::span<uint8_t> data, uint8_t dev_addr)
{
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
        // Wait for transfer
        while (!(_base_addr->ISR & I2C_ISR_RXNE))
        {
        }

        byte = _base_addr->RXDR;
    }

    return true;
}

bool HwI2c::write(std::span<const uint8_t> data, uint8_t dev_addr)
{
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
    _base_addr->CR2 |=
        ((data.size() << (I2C_CR2_NBYTES_Pos + 1)) | I2C_CR2_AUTOEND);

    // Initiate write
    _base_addr->CR2 |= I2C_CR2_START;

    // Write
    for (const uint8_t byte : data)
    {
        // Wait for transfer or NACK
        while (!(_base_addr->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF)))
        {
            if (_base_addr->ISR & I2C_ISR_NACKF)
            {
                _base_addr->ICR |= I2C_ICR_NACKCF;
                return false;
            }
        }
        _base_addr->TXDR = byte;
    }

    return true;
}
}  // namespace Stml4
}  // namespace LBR