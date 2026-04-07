#include <cstdint>
#include "board.h"
#include "st_gpio.h"
#include "st_i2c.h"

namespace LBR
{
// For 64 MHZ, got this val from STM32CubeMX
static constexpr uint32_t kTimingR{0x20303E5D};

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
const Stmh7::StI2cParams i2c_params{I2C1, kTimingR};

// Create I2C, SCL pin, and SDA pin objects
Stmh7::HwI2c i2c(i2c_params);
Stmh7::HwGpio scl(scl_params);
Stmh7::HwGpio sda(sda_params);

Board board{.i2c = i2c};

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

    return ret;
}

Board& get_board()
{
    return board;
}

}  // namespace LBR