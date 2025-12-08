/**
* @file imu_app_bsp.cc
* @brief IMU application board support package implementation.
*/

#include "../board.h"
#include "st_i2c.h"
#include "st_gpio.h"
#include "stm32l4xx.h"
#include "delay.h"

namespace LBR {


bool bsp_init()
{
    // Enable GPIOB and I2C1 clocks
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

    // Configure PB8 = I2C1_SCL, PB9 = I2C1_SDA (AF4, open-drain, pull-up)
    LBR::Stml4::StGpioSettings gpio_settings{
        LBR::Stml4::GpioMode::ALT_FUNC,
        LBR::Stml4::GpioOtype::OPEN_DRAIN,
        LBR::Stml4::GpioOspeed::HIGH,
        LBR::Stml4::GpioPupd::PULL_UP,
        4 // AF4 for I2C1
    };

    // Initialize GPIO pins for I2C
    LBR::Stml4::StGpioParams scl{gpio_settings, 8, GPIOB};
    LBR::Stml4::StGpioParams sda{gpio_settings, 9, GPIOB};
    LBR::Stml4::HwGpio scl_gpio(scl);
    LBR::Stml4::HwGpio sda_gpio(sda);
    scl_gpio.init();
    sda_gpio.init();

    // I2C timing register for ~100kHz (depends on system clock). Placeholder typical value.
    static constexpr uint32_t TIMINGR_100KHZ = 0x20303E5D;
    static LBR::Stml4::StI2cParams i2c_params{I2C1, TIMINGR_100KHZ};

    // Create I2C instance and initialize
    static LBR::Stml4::HwI2c i2c(i2c_params);
    i2c.init();

    // --- Add PA0 as reset pin ---
    LBR::Stml4::StGpioSettings rst_settings{
        LBR::Stml4::GpioMode::GPOUT,
        LBR::Stml4::GpioOtype::PUSH_PULL,
        LBR::Stml4::GpioOspeed::LOW,
        LBR::Stml4::GpioPupd::NO_PULL,
        0};
    LBR::Stml4::StGpioParams rst_params{rst_settings, 0, GPIOA};
    static LBR::Stml4::HwGpio rst(rst_params);
    rst.init();
    // BNO055 reset sequence
    rst.set(false); // Hold BNO055 in reset
    LBR::Utils::DelayMs(10);
    rst.set(true);  // Release reset
    LBR::Utils::DelayMs(650); // Wait for BNO055 to boot

    // Construct IMU driver with repeated start callback for bare-metal safety
    static Bno055 imu(i2c, Bno055::ADDR_PRIMARY);
    imu.init();

    return true;
}

Board& get_board()
{
    // Enable GPIOB and I2C1 clocks
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

    // Configure PB8 = I2C1_SCL, PB9 = I2C1_SDA (AF4, open-drain, pull-up)
    LBR::Stml4::StGpioSettings gpio_settings{
        LBR::Stml4::GpioMode::ALT_FUNC,
        LBR::Stml4::GpioOtype::OPEN_DRAIN,
        LBR::Stml4::GpioOspeed::HIGH,
        LBR::Stml4::GpioPupd::PULL_UP,
        4 // AF4 for I2C1
    };

    // Initialize GPIO pins for I2C
    LBR::Stml4::StGpioParams scl{gpio_settings, 8, GPIOB};
    LBR::Stml4::StGpioParams sda{gpio_settings, 9, GPIOB};
    LBR::Stml4::HwGpio scl_gpio(scl);
    LBR::Stml4::HwGpio sda_gpio(sda);
    scl_gpio.init();
    sda_gpio.init();

    // I2C timing register for ~100kHz
    static constexpr uint32_t TIMINGR_100KHZ = 0x20303E5D;
    static LBR::Stml4::StI2cParams i2c_params{I2C1, TIMINGR_100KHZ};

    static LBR::Stml4::HwI2c i2c(i2c_params);
    i2c.init();

    static Bno055 imu(i2c, Bno055::ADDR_PRIMARY);
    imu.init();

    static Board board{.imu = imu};
    return board;
}

} // namespace LBR