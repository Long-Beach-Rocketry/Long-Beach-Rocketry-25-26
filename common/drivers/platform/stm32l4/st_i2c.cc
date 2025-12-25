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
    bool ret{true};

    ret = ret && initiate_transfer(dev_addr, GENERATE_START_ONLY, WRITE, 1);
    std::array<uint8_t, 1> reg{reg_addr};
    std::span<const uint8_t> register_addr{reg};
    ret = ret && burst_write(register_addr);
    ret = ret &&
          initiate_transfer(dev_addr, GENERATE_STARTSTOP, READ, data.size());
    ret = ret && burst_read(data);
    return ret && detect_stop();
}

bool HwI2c::mem_read(std::span<uint8_t> data, const uint16_t reg_addr,
                     uint8_t dev_addr)
{
    bool ret{true};

    ret = ret && initiate_transfer(dev_addr, GENERATE_START_ONLY, WRITE, 2);
    std::array<uint8_t, 2> reg{static_cast<uint8_t>((reg_addr >> 8) & 0xFF),
                               static_cast<uint8_t>(reg_addr & 0xFF)};
    std::span<const uint8_t> register_addr{reg};
    ret = ret && burst_write(register_addr);
    ret = ret &&
          initiate_transfer(dev_addr, GENERATE_STARTSTOP, READ, data.size());
    ret = ret && burst_read(data);
    return ret && detect_stop();
}

bool HwI2c::mem_write(std::span<const uint8_t> data, const uint8_t reg_addr,
                      uint8_t dev_addr)
{
    bool ret{true};

    ret = ret && initiate_transfer(dev_addr, GENERATE_STARTSTOP, WRITE,
                                   1 + data.size());
    std::array<uint8_t, 1> reg{reg_addr};
    std::span<const uint8_t> register_addr{reg};
    ret = ret && burst_write(register_addr);
    ret = ret && burst_write(data);
    return ret && detect_stop();
}

bool HwI2c::mem_write(std::span<const uint8_t> data, const uint16_t reg_addr,
                      uint8_t dev_addr)
{
    bool ret{true};

    ret = ret && initiate_transfer(dev_addr, GENERATE_STARTSTOP, WRITE,
                                   2 + data.size());
    std::array<uint8_t, 2> reg{static_cast<uint8_t>((reg_addr >> 8) & 0xFF),
                               static_cast<uint8_t>(reg_addr & 0xFF)};
    std::span<const uint8_t> register_addr{reg};
    ret = ret && burst_write(register_addr);
    ret = ret && burst_write(data);
    return ret && detect_stop();
}

bool HwI2c::initiate_transfer(uint8_t dev_addr, uint8_t mode, uint8_t dir,
                              size_t num_bytes)
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

    _base_addr->CR2 &=
        ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_SADD | I2C_CR2_AUTOEND);

    // Configure according to mode
    switch (mode)
    {
        case GENERATE_STARTSTOP:
            _base_addr->CR2 |= I2C_CR2_AUTOEND;
            break;
        case GENERATE_START_ONLY:
            break;
        case GENERATE_STOP_ONLY:
            _base_addr->CR2 |= I2C_CR2_STOP;
            return detect_stop();
        default:
            return false;
    }

    // Configuring and initiating transfer
    _base_addr->CR2 |=
        ((num_bytes << I2C_CR2_NBYTES_Pos) | (dir << I2C_CR2_RD_WRN_Pos) |
         (dev_addr << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_START);

    return true;
}

bool HwI2c::burst_read(std::span<uint8_t> data)
{
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

bool HwI2c::burst_write(std::span<const uint8_t> data)
{
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

bool HwI2c::detect_stop()
{
    while (!(_base_addr->ISR & I2C_ISR_STOPF))
    {
    }
    _base_addr->ICR |= I2C_ICR_STOPCF;

    return true;
}

}  // namespace Stml4
}  // namespace LBR