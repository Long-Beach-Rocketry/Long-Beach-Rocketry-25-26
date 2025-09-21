#include "st_i2c_module.h"

namespace LBR
{
namespace Stml4
{
I2cModule::I2cModule() : _i2c{nullptr}
{
}

bool I2cModule::GiveI2c(I2C_TypeDef* base_addr, uint32_t timingr,
                        const StGpioParams& sda_pin,
                        const StGpioParams& scl_pin)
{
    bool ret;

    HwGpio sda_gpio = HwGpio(sda_pin);
    HwGpio scl_gpio = HwGpio(scl_pin);
    HwI2c i2c = HwI2c(base_addr, timingr);

    _i2c = &i2c;

    ret = sda_gpio.init();
    if (ret == false)
    {
        return false;
    }

    ret = scl_gpio.init();
    if (ret == false)
    {
        return false;
    }

    ret = i2c.init();
    if (ret == false)
    {
        return false;
    }

    return true;
}

HwI2c* I2cModule::CreateI2c()
{
    return _i2c;
}
}  // namespace Stml4
}  // namespace LBR