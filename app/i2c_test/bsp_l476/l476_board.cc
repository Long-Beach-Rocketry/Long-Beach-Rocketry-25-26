#include <cstdint>
#include "board.h"
#include "st_i2c_module.h"

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

Stml4::I2cModule i2c(i2c_params, sda_params, scl_params);

Board board{.i2c = i2c.GetI2c()};

Board& board_init()
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

    i2c.InitI2c();

    return board;
}

}  // namespace LBR