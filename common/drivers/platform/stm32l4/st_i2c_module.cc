#include "st_i2c_module.h"

namespace LBR
{
namespace Stml4
{
I2cModule::I2cModule(const StI2cParams& params, const StGpioParams& sda_pin,
                     const StGpioParams& scl_pin)
    : _i2c_params{params}, _sda_pin{sda_pin}, _scl_pin{scl_pin}
{
}

HwI2c& I2cModule::CreateI2c()
{
    HwGpio sda_gpio(_sda_pin);
    HwGpio scl_gpio(_scl_pin);
    static HwI2c i2c(_i2c_params);

    sda_gpio.init();
    scl_gpio.init();
    i2c.init();

    return i2c;
}
}  // namespace Stml4
}  // namespace LBR