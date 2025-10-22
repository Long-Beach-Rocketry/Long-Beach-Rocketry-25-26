#include "st_i2c_module.h"

namespace LBR
{
namespace Stml4
{
I2cModule::I2cModule(const StI2cParams& params, const StGpioParams& sda_pin,
                     const StGpioParams& scl_pin)
    : _i2c_params{params}, _sda_pin{sda_pin}, _scl_pin{scl_pin}, _i2c{params}
{
}

bool I2cModule::InitI2c()
{
    HwGpio sda_gpio(_sda_pin);
    HwGpio scl_gpio(_scl_pin);

    bool ret;

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

    ret = _i2c.init();
    if (ret == false)
    {
        return false;
    }

    return true;
}

HwI2c& I2cModule::GetI2c()
{
    return _i2c;
}
}  // namespace Stml4
}  // namespace LBR