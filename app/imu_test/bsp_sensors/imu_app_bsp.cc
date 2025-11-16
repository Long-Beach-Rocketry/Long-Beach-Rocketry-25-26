/**
* @file imu_app_bsp.cc
* @brief IMU application board support package implementation.
*/

#include "imu_app_bsp.h"
#include "st_i2c.h"
#include "st_gpio.h"
#include "stm32l4xx.h"

namespace LBR {

Bno055& bsp_init_imu()
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

    // Construct IMU driver (I2C + address)
    static Bno055 imu(i2c, Bno055::ADDR_PRIMARY);
    imu.init();

    return imu;
}

} // namespace LBR