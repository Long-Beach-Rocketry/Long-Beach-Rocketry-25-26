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

enum TransferMode : uint8_t
{
    GENERATE_STARTSTOP = 0,
    GENERATE_START_ONLY,
    GENERATE_STOP_ONLY
};

enum Dir : uint8_t
{
    WRITE = 0,
    READ
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

    bool mem_read(std::span<uint8_t> data, const uint8_t reg_addr,
                  uint8_t dev_addr) override;
    bool mem_read(std::span<uint8_t> data, const uint16_t reg_addr,
                  uint8_t dev_addr) override;
    bool mem_write(std::span<const uint8_t> data, const uint8_t reg_addr,
                   uint8_t dev_addr) override;
    bool mem_write(std::span<const uint8_t> data, const uint16_t reg_addr,
                   uint8_t dev_addr) override;

private:
    I2C_TypeDef* _base_addr;
    uint32_t _timingr;

    bool initiate_transfer(uint8_t dev_addr, uint8_t mode, uint8_t dir,
                           size_t num_bytes) override;
    bool detect_stop() override;

    bool burst_read(std::span<uint8_t> data) override;
    bool burst_write(std::span<const uint8_t> data) override;
};
}  // namespace Stml4
}  // namespace LBR