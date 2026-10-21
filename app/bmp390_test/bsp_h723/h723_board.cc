#include <cstdint>
#include "bmp390.h"
#include "board.h"
#include "st_gpio.h"
#include "st_i2c.h"

namespace LBR
{
// SCL pin config (PB8)
Stmh7::StGpioSettings scl_settings{
    Stmh7::GpioMode::ALT_FUNC, Stmh7::GpioOtype::OPEN_DRAIN,
    Stmh7::GpioOspeed::LOW, Stmh7::GpioPupd::PULL_UP, 4};

const Stmh7::StGpioParams scl_params{scl_settings, 8, GPIOB};

// SDA pin config (PB9)
Stmh7::StGpioSettings sda_settings{
    Stmh7::GpioMode::ALT_FUNC, Stmh7::GpioOtype::OPEN_DRAIN,
    Stmh7::GpioOspeed::LOW, Stmh7::GpioPupd::PULL_UP, 4};

const Stmh7::StGpioParams sda_params{sda_settings, 9, GPIOB};

// I2C config
static constexpr uint32_t kTimingR{0x20303E5D};
const Stmh7::StI2cParams i2c_params{I2C1, kTimingR};

// Create I2C, SCL pin, and SDA pin objects
Stmh7::HwI2c i2c(i2c_params);
Stmh7::HwGpio scl(scl_params);
Stmh7::HwGpio sda(sda_params);

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
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    RCC->APB1LENR |= RCC_APB1LENR_I2C1EN;

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