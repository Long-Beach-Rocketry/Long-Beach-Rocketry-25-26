#include <array>
#include "board.h"
#include "st_gpio.h"
#include "st_i2c.h"
#include "st_sysclk.h"

static constexpr uint32_t kSysclkHz{8'000'000};
static constexpr uint32_t kBaudRate{115'200};
static constexpr uint32_t kI2cTimingR{0x00303D5B};

namespace LBR
{
namespace Stmh7
{

/**
 * @brief GPIO Registry
 * PA0 - IMU reset
 * PB8 - I2c SCL
 * PB9 - I2c SDA
 */

// Create Clock object (Sysclk = 65 MHz, HCLK = 32.5 MHz, PCLK1 = PCLK2 = PCLK3 = PCLK4 = 16.25 MHz)
ClockParams clock_params{Source::HSE8_MHZ_BYPASS, kSysclkHz,
                         D1cprePrescaler::DIV1,   AhbPrescaler::DIV2,
                         Apb1Prescaler::DIV2,     Apb2Prescaler::DIV2,
                         Apb3Prescaler::DIV2,     Apb4Prescaler::DIV2};
HwClock clock{clock_params};

// Configure I2c
StI2cParams i2c_params{I2C1, kI2cTimingR};

HwI2c i2c(i2c_params);

// Set up BARO pins (SCL PB8, SDA PB9)
StGpioSettings sda_settings{GpioMode::ALT_FUNC, GpioOtype::OPEN_DRAIN,
                            GpioOspeed::LOW, GpioPupd::PULL_UP, 4};
StGpioParams sda_params{sda_settings, 9, GPIOB};
HwGpio sda{sda_params};

StGpioSettings scl_settings{GpioMode::ALT_FUNC, GpioOtype::OPEN_DRAIN,
                            GpioOspeed::LOW, GpioPupd::PULL_UP, 4};
StGpioParams scl_params{scl_settings, 8, GPIOB};
HwGpio scl{scl_params};

// Reset pin for BNO055 (PA0)
Stmh7::StGpioSettings rst_settings{
    Stmh7::GpioMode::GPOUT, Stmh7::GpioOtype::PUSH_PULL, Stmh7::GpioOspeed::LOW,
    Stmh7::GpioPupd::NO_PULL, 0};
const Stmh7::StGpioParams rst_params{rst_settings, 0, GPIOA};
Stmh7::HwGpio rst(rst_params);

// Create BNO055 IMU object
Bno055 imu(static_cast<LBR::I2c&>(i2c), Bno055::ADDR_PRIMARY);

// Create Barometer object
Bmp390Params baro_params{i2c, 0x76};
Bmp390 baro{baro_params};

}  // namespace Stmh7

Board board{.clock = Stmh7::clock,
            .bno055 = Stmh7::imu,
            .bmp390 = Stmh7::baro};

bool board_init()
{
    bool ret = true;

    // Enable peripheral clocks
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN | RCC_AHB4ENR_GPIOBEN |
                    RCC_AHB4ENR_GPIODEN | RCC_AHB4ENR_GPIOEEN;
    RCC->APB1LENR |= RCC_APB1LENR_USART3EN | RCC_APB1LENR_I2C1EN;

    // Configure clock tree for this test.
    bool clock_ok = Stmh7::clock.init();
    ret &= clock_ok;

    ret &= Stmh7::sda.init();
    ret &= Stmh7::scl.init();

    ret &= Stmh7::i2c.init();

    ret &= Stmh7::rst.init();
    ret &= Stmh7::rst.set(false);
    Utils::DelayMs(10);
    ret &= Stmh7::rst.set(true);
    Utils::DelayMs(650);
    Stmh7::imu.init();

    ret &= Stmh7::baro.init();

    return ret;
}

Board& get_board()
{
    return board;
}

extern "C" void IncDelayTicks(void);

extern "C" void SysTick_Handler(void)
{
    HAL_IncTick();
    IncDelayTicks();
}

}  // namespace LBR