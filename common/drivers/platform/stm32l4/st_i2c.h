#pragma once

#include <cstdint>
#include <span>
#include "i2c.h"
#include "stm32l476xx.h"

namespace LBR
{
namespace Stml4
{
class HwI2c : public I2c
{

public:
    explicit HwI2c(I2C_TypeDef* instance, uint32_t timingr);

    /**
     * @brief Initializes I2C peripheral;
     * 
     * @param None
     * @return true if successful, false otherwise
     */
    bool init();

    bool read(std::span<uint8_t> data, uint8_t dev_addr) override;
    bool write(std::span<const uint8_t> data, uint8_t dev_addr) override;

private:
    I2C_TypeDef* _instance;
    uint32_t _timingr;
};
}  // namespace Stml4
}  // namespace LBR