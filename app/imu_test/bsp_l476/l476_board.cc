#include <stdint.h>
#include "board.h"
#include "delay.h"
#include "st_gpio.h"
#include "st_i2c.h"
#include "st_sys_clock.h"

namespace LBR
{

// SCL pin config (PB8)
Stml4::StGpioSettings scl_settings{
    Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::OPEN_DRAIN,
    Stml4::GpioOspeed::HIGH, Stml4::GpioPupd::PULL_UP, 4};
const Stml4::StGpioParams scl_params{scl_settings, 8, GPIOB};

// SDA pin config (PB9)
Stml4::StGpioSettings sda_settings{
    Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::OPEN_DRAIN,
    Stml4::GpioOspeed::HIGH, Stml4::GpioPupd::PULL_UP, 4};
const Stml4::StGpioParams sda_params{sda_settings, 9, GPIOB};

// I2C config
const Stml4::StI2cParams i2c_params{I2C1, 0x20303E5D};

// Create I2C, SCL pin, and SDA pin objects
Stml4::HwI2c i2c(i2c_params);
Stml4::HwGpio scl(scl_params);
Stml4::HwGpio sda(sda_params);

// Add PA0 as reset pin for BNO055
Stml4::StGpioSettings rst_settings{
    Stml4::GpioMode::GPOUT, Stml4::GpioOtype::PUSH_PULL, Stml4::GpioOspeed::LOW,
    Stml4::GpioPupd::NO_PULL, 0};
const Stml4::StGpioParams rst_params{rst_settings, 0, GPIOA};
Stml4::HwGpio rst(rst_params);

// Construct IMU driver using generic I2c interface
Bno055 imu(static_cast<LBR::I2c&>(i2c), Bno055::ADDR_PRIMARY);
Board board{.imu = imu};

bool bsp_init()
{
    // Initialize system clock first
    static LBR::Stml4::HwClock clock;
    clock.init(LBR::Stml4::HwClock::configuration::DEFAULT_4MHZ);

    // Enable peripheral clocks
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

    bool ret = true;
    ret = ret && sda.init();
    ret = ret && scl.init();
    ret = ret && i2c.init();
    ret = ret && rst.init();
    // BNO055 reset sequence
    ret = ret && rst.set(false);
    LBR::Utils::DelayMs(10);
    ret = ret && rst.set(true);
    LBR::Utils::DelayMs(650);
    // Initialize IMU
    imu.init();
    return ret;
}

Board& get_board()
{
    return board;
}

}  // namespace LBR
