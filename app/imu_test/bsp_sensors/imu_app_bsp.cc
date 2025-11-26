/**
* @file imu_app_bsp.cc
* @brief IMU application board support package implementation.
*/

#include "../board.h"
#include "st_i2c.h"
#include "st_gpio.h"
#include "stm32l4xx.h"

namespace LBR {

// Repeated start I2C helper for BNO055 sensor reads 
static bool bsp_i2c_mem_read(uint8_t dev_addr, uint8_t reg, uint8_t* data, size_t len)
{
    I2C_TypeDef* i2c_base = I2C1;
    
    // Phase 1: Write register address (without AUTOEND for repeated start)
    i2c_base->CR2 &= ~I2C_CR2_SADD;
    i2c_base->CR2 |= (dev_addr << (I2C_CR2_SADD_Pos + 1));
    i2c_base->CR2 &= ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND);
    i2c_base->CR2 |= (1 << I2C_CR2_NBYTES_Pos);
    i2c_base->CR2 |= I2C_CR2_START;
    
    // Write register address
    while (!(i2c_base->ISR & I2C_ISR_TXIS))
    {
        if (i2c_base->ISR & I2C_ISR_NACKF)
        {
            i2c_base->ICR |= I2C_ICR_NACKCF;
            return false;
        }
    }
    i2c_base->TXDR = reg;
    
    // Wait for TC (Transfer Complete)
    while (!(i2c_base->ISR & I2C_ISR_TC))
    {
    }
    
    // Phase 2: Repeated START for read (no STOP between)
    i2c_base->CR2 &= ~I2C_CR2_NBYTES;
    i2c_base->CR2 |= (len << I2C_CR2_NBYTES_Pos) | I2C_CR2_RD_WRN | 
                     I2C_CR2_AUTOEND | I2C_CR2_START;
    
    // Read data
    for (size_t i = 0; i < len; i++)
    {
        while (!(i2c_base->ISR & I2C_ISR_RXNE))
        {
        }
        data[i] = i2c_base->RXDR;
    }
    
    // Wait for STOP
    while (!(i2c_base->ISR & I2C_ISR_STOPF))
    {
    }
    i2c_base->ICR |= I2C_ICR_STOPCF;
    
    return true;
}

// Repeated start I2C write helper for register writes
static bool bsp_i2c_mem_write(uint8_t dev_addr, uint8_t reg, const uint8_t* data, size_t len)
{
    I2C_TypeDef* i2c_base = I2C1;
    
    // Write register address + data in single transaction
    i2c_base->CR2 &= ~I2C_CR2_SADD;
    i2c_base->CR2 |= (dev_addr << (I2C_CR2_SADD_Pos + 1));
    i2c_base->CR2 &= ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN);
    i2c_base->CR2 |= ((1 + len) << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND;
    i2c_base->CR2 |= I2C_CR2_START;
    
    // Write register address
    while (!(i2c_base->ISR & I2C_ISR_TXIS))
    {
        if (i2c_base->ISR & I2C_ISR_NACKF)
        {
            i2c_base->ICR |= I2C_ICR_NACKCF;
            return false;
        }
    }
    i2c_base->TXDR = reg;
    
    // Write data
    for (size_t i = 0; i < len; i++)
    {
        while (!(i2c_base->ISR & I2C_ISR_TXIS))
        {
            if (i2c_base->ISR & I2C_ISR_NACKF)
            {
                i2c_base->ICR |= I2C_ICR_NACKCF;
                return false;
            }
        }
        i2c_base->TXDR = data[i];
    }
    
    // Wait for STOP
    while (!(i2c_base->ISR & I2C_ISR_STOPF))
    {
    }
    i2c_base->ICR |= I2C_ICR_STOPCF;
    
    return true;
}

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

    // Construct IMU driver with repeated start callback for bare-metal safety
    static Bno055 imu(i2c, Bno055::ADDR_PRIMARY, bsp_i2c_mem_read, bsp_i2c_mem_write);
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

    static Bno055 imu(i2c, Bno055::ADDR_PRIMARY, bsp_i2c_mem_read, bsp_i2c_mem_write);
    imu.init();

    static Board board{.imu = imu};
    return board;
}

} // namespace LBR