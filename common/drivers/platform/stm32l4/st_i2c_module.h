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
     * @param params struct containing base address and timing register value for the I2C instance
     * @param sda_pin struct containing settings, pin number, and port address for SDA pin
     * @param scl_pin struct containing settings, pin number, and port address for SCL pin
     */
    explicit I2cModule(const StI2cParams& params, const StGpioParams& sda_pin,
                       const StGpioParams& scl_pin);
    ;


    bool InitI2c();
    /**
     * @brief Creates HwI2c instance
     * 
     * @return reference to HwI2c instance
     */
    HwI2c& GetI2c();

private:
    const StI2cParams _i2c_params;
    const StGpioParams& _sda_pin;
    const StGpioParams& _scl_pin;
    HwI2c _i2c;
};
}  // namespace Stml4
}  // namespace LBR