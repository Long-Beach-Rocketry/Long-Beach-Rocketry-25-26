#include <stdint.h>
#include "board.h"
#include "delay.h"
#include "st_gpio.h"
#include "st_i2c.h"
#include "stm32h7xx_hal.h"  // for the hal_init
// #include "st_sys_clock.h"

namespace LBR
{

/* 
* Functions that were supposed to be in st_sys_clock. REMOVE both IncDelayTicks()
* and SysTick_Handler() when st_sys_clk finished.
*
* Also uncomment the #include "st_sys_clock.h"
*
* Also remove the #include "stm32h7xx_hal.h".
*
* Also remove the HAL_Init(); in the bsp_init() function.
*
* The DelayMs in the bsp_init() depends on g_ms_ticks getting updated using 
* IncDelayTicks() which is called in the SysTick_Handler, a function in
* the st_sys_clk which we don't have yet.
*/

extern "C" void IncDelayTicks(void);

extern "C" void SysTick_Handler(void)
{
    HAL_IncTick();    // for HAL
    IncDelayTicks();  // increments g_ms_ticks
}
// For 64 MHZ, got this val from STM32CubeMX. REMOVE when st_sys_clk finished
static constexpr uint32_t kTimingR{0x20303E5D};

// SCL pin config (PB8)
Stmh7::StGpioSettings scl_settings{
    Stmh7::GpioMode::ALT_FUNC, Stmh7::GpioOtype::OPEN_DRAIN,
    Stmh7::GpioOspeed::HIGH, Stmh7::GpioPupd::PULL_UP, 4};
const Stmh7::StGpioParams scl_params{scl_settings, 8, GPIOB};

// SDA pin config (PB9)
Stmh7::StGpioSettings sda_settings{
    Stmh7::GpioMode::ALT_FUNC, Stmh7::GpioOtype::OPEN_DRAIN,
    Stmh7::GpioOspeed::HIGH, Stmh7::GpioPupd::PULL_UP, 4};
const Stmh7::StGpioParams sda_params{sda_settings, 9, GPIOB};

// I2C config
const Stmh7::StI2cParams i2c_params{I2C1, kTimingR};

// Create I2C, SCL pin, and SDA pin objects
Stmh7::HwI2c i2c(i2c_params);
Stmh7::HwGpio scl(scl_params);
Stmh7::HwGpio sda(sda_params);

// Add PA0 as reset pin for BNO055
Stmh7::StGpioSettings rst_settings{
    Stmh7::GpioMode::GPOUT, Stmh7::GpioOtype::PUSH_PULL, Stmh7::GpioOspeed::LOW,
    Stmh7::GpioPupd::NO_PULL, 0};
const Stmh7::StGpioParams rst_params{rst_settings, 0, GPIOA};
Stmh7::HwGpio rst(rst_params);

// Construct IMU driver using generic I2c interface
Bno055 imu(static_cast<LBR::I2c&>(i2c), Bno055::ADDR_PRIMARY);
Board board{.imu = imu};

bool bsp_init()
{
    // Initialize system clock first; REMOVE when st_sys_clk finished
    HAL_Init();

    // Enable peripheral clocks
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    RCC->APB1LENR |= RCC_APB1LENR_I2C1EN;

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
