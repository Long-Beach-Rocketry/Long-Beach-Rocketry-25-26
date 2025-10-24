/**
* @file imu_app_bsp.cc
* @brief IMU application board support package implementation.
* @author Bex Saw
* @date 2025-10-24
*/

#include "imu_app_bsp.h"
#include "st_i2c_module.h"
#include "delay.h"
#include "st_gpio.h"

LBR::Bno055& BSP_Init_IMU(I2c_TypeDef* i2c_instance, GPIO_TypeDef* gpio_instance) {

    (void)gpio_instance;

     // Enable GPIOB clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    // Enable I2C1 clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

    LBR::Stml4::StGpioSettings gpio_settings{
        LBR::Stml4::GpioMode::ALT_FUNC,
        LBR::Stml4::GpioOtype::OPEN_DRAIN,
        LBR::Stml4::GpioOspeed::HIGH,
        LBR::Stml4::GpioPupd::PULL_UP,

    };

    LBR::StGpioParams scl_params{gpio_settings, 8, GPIOB}; // PB8
    LBR::StGpioParams sda_params{gpio_settings, 9, GPIOB}; // PB9

    // I2C Params
    LBR::StI2cParams i2c_params{i2c_instance, 100000}; // 100kHz


    //Create I2C object
    static LBR::Stml4::StI2cModule i2c_module(i2c_params, sda_pin, scl_pin);
    i2c_module.InitI2c();

    // Create IMU object
    static LBR::Bno055 imu(i2c_module.GetI2c(),0x28); // IMU I2C address 0x28
    imu.Init();
    }
