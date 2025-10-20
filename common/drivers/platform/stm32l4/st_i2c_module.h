/**
 * @file st_i2c_module.h
 * @brief I2C Module to create I2C objects for the stml4
 * @author Yshi Blanco
 * @date 10/13/2025
 */

#pragma once

#include <cstdint>
#include "st_gpio.h"
#include "st_i2c.h"
#include "stm32l476xx.h"

namespace LBR
{
namespace Stml4
{
class I2cModule
{
public:
    /**
     * @brief I2cModule constructor
     * 
     * @param base_addr base address of I2C peripheral
     * @param timingr timing settings
     * @param sda_pin struct containing settings, pin number, and port address for SDA pin
     * @param scl_pin struct containing settings, pin number, and port address for SCL pin
     */
    explicit I2cModule(I2C_TypeDef* base_addr, uint32_t timingr,
                       const StGpioParams& sda_pin,
                       const StGpioParams& scl_pin);
    ;

    /**
     * @brief Creates HwI2c instance
     * 
     * @return reference to HwI2c instance
     */
    HwI2c& CreateI2c();

private:
    I2C_TypeDef* _base_addr;
    uint32_t _timingr;
    const StGpioParams& _sda_pin;
    const StGpioParams& _scl_pin;
};
}  // namespace Stml4
}  // namespace LBR