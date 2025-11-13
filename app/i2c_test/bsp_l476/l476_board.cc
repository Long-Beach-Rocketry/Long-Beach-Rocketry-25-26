#include <cstdint>
#include "board.h"
#include "st_gpio.h"
#include "st_i2c.h"

namespace LBR
{
// SCL pin config (PB8)
Stml4::StGpioSettings scl_settings{
    Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::OPEN_DRAIN,
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::PULL_UP, 4};

const Stml4::StGpioParams scl_params{scl_settings, 8, GPIOB};

// SDA pin config (PB9)
Stml4::StGpioSettings sda_settings{
    Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::OPEN_DRAIN,
    Stml4::GpioOspeed::LOW, Stml4::GpioPupd::PULL_UP, 4};

const Stml4::StGpioParams sda_params{scl_settings, 9, GPIOB};

// I2C config
const Stml4::StI2cParams i2c_params{I2C1, 0x10909CEC};

// Create I2C, SCL pin, and SDA pin objects
Stml4::HwI2c i2c(i2c_params);
Stml4::HwGpio scl(scl_params);
Stml4::HwGpio sda(sda_params);

Board board{.i2c = i2c};

bool bsp_init()
{
    // Enable peripheral clocks
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

    // Initialize I2C and pins
    bool ret;

    ret = sda.init();
    if (ret == false)
    {
        return false;
    }

    ret = scl.init();
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

Board& get_board()
{
    return board;
}

}  // namespace LBR