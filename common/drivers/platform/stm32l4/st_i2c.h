/**
 * @file st_i2c.h
 * @brief I2C driver for STM32L476xx
 * @author Yshi Blanco
 * @date 10/02/2025
 */

#pragma once

#include <cstdint>
#include <span>
#include "i2c.h"
#include "stm32l476xx.h"

namespace LBR
{
namespace Stml4
{

/**
 * @brief Collection of base address and timing register info
 */
struct StI2cParams
{
    I2C_TypeDef* base_addr;
    uint32_t timingr;
};

class HwI2c : public I2c
{

public:
    explicit HwI2c(const StI2cParams& params);

    /**
     * @brief Initializes I2C peripheral;
     * 
     * @param None
     * @return true if successful, false otherwise
     */
    bool init();

private:
    I2C_TypeDef* _base_addr;
    uint32_t _timingr;

    bool burst_read(std::span<uint8_t> data, uint8_t dev_addr) override;
    bool burst_write(std::span<const uint8_t> data, uint8_t dev_addr) override;
};
}  // namespace Stml4
}  // namespace LBR