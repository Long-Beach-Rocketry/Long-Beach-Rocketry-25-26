#include "st_i2c.h"

namespace LBR
{
namespace Stml4
{
HwI2c::HwI2c(I2C_TypeDef* instance, uint32_t timingr)
    : _instance{instance}, _timingr{timingr}
{
}

bool HwI2c::init()
{
    if (_instance == nullptr)
    {
        return false;
    }

    // Reset peripheral
    _instance->CR1 &= ~I2C_CR1_PE;

    // Configure timing
    _instance->TIMINGR |= _timingr;
    _instance->TIMINGR &= _timingr;

    _instance->CR2 &= ~I2C_CR2_ADD10;  // 7-bit addressing mode

    // Enable peripheral
    _instance->CR1 |= I2C_CR1_PE;

    return true;
}

bool HwI2c::read(std::span<uint8_t> data, uint8_t dev_addr)
{
    /**
     * Setting target
     * Checking if communication in progress
     */
    if (_instance->CR2 & I2C_CR2_START)
    {
        return false;
    }

    _instance->CR2 &= ~I2C_CR2_SADD;                         // Clearing address
    _instance->CR2 |= (dev_addr << (I2C_CR2_SADD_Pos + 1));  // Setting address

    // Make sure I2C bus is idle
    if (_instance->ISR & I2C_ISR_BUSY)
    {
        return false;
    }

    // Configure for reading
    _instance->CR2 &= ~I2C_CR2_NBYTES;
    _instance->CR2 |= ((data.size() << I2C_CR2_NBYTES_Pos) | I2C_CR2_RD_WRN |
                       I2C_CR2_AUTOEND);

    // Initiate read
    _instance->CR2 |= I2C_CR2_START;

    for (uint8_t& byte : data)
    {
        // Wait for transfer
        while (!(_instance->ISR & I2C_ISR_RXNE))
        {
        }

        byte = _instance->RXDR;
    }

    return true;
}

bool HwI2c::write(std::span<const uint8_t> data, uint8_t dev_addr)
{
    /**
             * Setting target
             * Check if communication is in progress
             */
    if (_instance->CR2 & I2C_CR2_START)
    {
        return false;
    }

    _instance->CR2 &= ~I2C_CR2_SADD;                         // Clearing address
    _instance->CR2 |= (dev_addr << (I2C_CR2_SADD_Pos + 1));  // Setting address

    // Make sure I2C bus is idle
    if (_instance->ISR & I2C_ISR_BUSY)
    {
        return false;
    }

    // Configure for writing
    _instance->CR2 &= ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN);
    _instance->CR2 |= ((data.size() << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND);

    // Initiate write
    _instance->CR2 |= I2C_CR2_START;

    // Write
    for (const uint8_t byte : data)
    {
        // Wait for transfer or NACK
        while (!(_instance->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF)))
        {
            if (_instance->ISR & I2C_ISR_NACKF)
            {
                _instance->ICR |= I2C_ICR_NACKCF;
                return false;
            }

            _instance->TXDR = byte;
        }
    }

    return true;
}
}  // namespace Stml4
}  // namespace LBR