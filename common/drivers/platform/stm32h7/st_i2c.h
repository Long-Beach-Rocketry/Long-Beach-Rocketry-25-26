/**
 * @file st_i2c.h
 * @brief I2C driver specfic for the stmh7.
 * @author Joseph Chang
 */

#pragma once

#include "stm32h723xx.h"
#include "i2c.h"
#include <cstdint>
#include <array>

namespace LBR
{
namespace Stmh7
{
  struct StI2cParams {
    I2C_TypeDef* base_addr;
    uint32_t timingr;
  };

  class HwI2c : public I2c {

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

    bool read(std::span<uint8_t> data, uint8_t dev_addr) override;

    bool write(std::span<const uint8_t> data, uint8_t dev_addr) override;

    private:
    I2C_TypeDef* m_base_addr;
    uint32_t m_timingr;
  };

}  // namespace Stmh7
}  // namespace LBR