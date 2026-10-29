#include <cstdint>
#include "board.h"
#include "st_gpio.h"
#include "st_i2c.h"

extern "C" void IncDelayTicks(void);

extern "C" void SysTick_Handler(void)
{
    IncDelayTicks();
}

namespace LBR
{
// SCL pin config (PB8)
Stmh7::StGpioSettings scl_settings{
    Stmh7::GpioMode::ALT_FUNC, Stmh7::GpioOtype::OPEN_DRAIN,
    Stmh7::GpioOspeed::LOW, Stmh7::GpioPupd::PULL_UP, 4};

//config pin
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

// Create MS5611 object
static constexpr uint8_t kMs5611Address{0x77};
const Ms561101Params barometer_params{i2c, kMs5611Address};
Ms561101 barometer(barometer_params);

Board board{.i2c = i2c, .barometer = barometer};

bool bsp_init()
{
    // Start the millisecond timebase
    SystemCoreClockUpdate();

    if (SysTick_Config(SystemCoreClock / 1000U) != 0U)
    {
        return false;
    }
    // Enable peripheral clocks
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    RCC->APB1LENR |= RCC_APB1LENR_I2C1EN;

    // Initialize I2C and pins
    const bool sda_ok = sda.init();
    const bool scl_ok = scl.init();
    const bool i2c_ok = i2c.init();
    const bool barometer_ok = barometer.init();

    return sda_ok && scl_ok && i2c_ok && barometer_ok;
}

Board& get_board()
{
    return board;
}

}  // namespace LBR