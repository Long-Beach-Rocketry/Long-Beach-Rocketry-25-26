#include <cstdint>
#include "bmp390.h"
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

const Stml4::StGpioParams sda_params{sda_settings, 9, GPIOB};

// I2C config
const Stml4::StI2cParams i2c_params{I2C1, 0x10909CEC};

// Create I2C, SCL pin, and SDA pin objects
Stml4::HwI2c i2c(i2c_params);
Stml4::HwGpio scl(scl_params);
Stml4::HwGpio sda(sda_params);

// Create BMP390 object
/**
 * @note SDO pin connection: Slave address LSB (GND = '0', VDDIO = '1')
 *          GND -> 0x76
 *          VDDIO -> 0x77
 */
const Bmp390Params bmp390_params{i2c, 0x76};
Bmp390 bmp390(bmp390_params);

Board board{.bmp390 = bmp390};

bool bsp_init()
{
    // Enable peripheral clocks
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

    // Initialize I2C and pins
    bool ret = true;

    ret = ret && sda.init();
    ret = ret && scl.init();
    ret = ret && i2c.init();

    // Initialize sensor
    ret = ret && bmp390.init();

    return ret;
}

Board& get_board()
{
    return board;
}

}  // namespace LBR