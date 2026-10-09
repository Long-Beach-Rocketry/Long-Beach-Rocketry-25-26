#include <cstdint>
#include "bmp390.h"
#include "bno055_imu.h"
#include "board.h"
#include "delay.h"
#include "st_gpio.h"
#include "st_i2c.h"
#include "st_sysclk.h"
#include "st_timebase.h"
#include "st_sensor_manager.h"

namespace LBR
{

extern "C" void IncDelayTicks(void);

extern "C" void SysTick_Handler(void)
{
    HAL_IncTick();    // for HAL
    IncDelayTicks();  // increments g_ms_ticks
}

static constexpr uint32_t kSysclkHz{64'000'000};
// For 64 MHz HSI, I2C timing value from STM32CubeMX
static constexpr uint32_t kTimingR{0x20303E5D};
// Default APB1 timer input frequency on HSI (64 MHz)
static constexpr uint32_t kTim3ClkHz{64'000'000};

// Clock config (Sysclk = 64 MHz HSI, AHB = 64 MHz, APB1..4 = 64 MHz)
Stmh7::ClockParams clock_params{
    Stmh7::Source::HSI64_MHZ,     kSysclkHz,
    Stmh7::D1cprePrescaler::DIV1, Stmh7::AhbPrescaler::DIV1,
    Stmh7::Apb1Prescaler::DIV1,   Stmh7::Apb2Prescaler::DIV1,
    Stmh7::Apb3Prescaler::DIV1,   Stmh7::Apb4Prescaler::DIV1};
Stmh7::HwClock clock{clock_params};

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

// Reset pin for BNO055 (PA0)
Stmh7::StGpioSettings rst_settings{
    Stmh7::GpioMode::GPOUT, Stmh7::GpioOtype::PUSH_PULL, Stmh7::GpioOspeed::LOW,
    Stmh7::GpioPupd::NO_PULL, 0};
const Stmh7::StGpioParams rst_params{rst_settings, 0, GPIOA};
Stmh7::HwGpio rst(rst_params);

// Create BMP390 object (default I2C address 0x76)
const Bmp390Params bmp390_params{i2c, 0x76};
Bmp390 bmp390(bmp390_params);

// Create BNO055 IMU object
Bno055 imu(static_cast<LBR::I2c&>(i2c), Bno055::ADDR_PRIMARY);

// TIM3 Timebase (16-bit, APB1)
const Stmh7::StTimebaseParams timebase_params{TIM3, TIM3_IRQn, kTim3ClkHz, true};
Stmh7::HwTimebase timebase(timebase_params);

// Sensors managed by the sensor manager
const Stmh7::StSensorMgrSensors sensor_mgr_sensors{
    .bno055 = &imu,
    .bmp390 = &bmp390,
};

const Stmh7::StSensorMgrParams sensor_mgr_params{
    .timebase = timebase,
    .sensors = sensor_mgr_sensors,
    .ekf = nullptr,  // TODO: no EKF instance yet, add once implemented
};

Stmh7::HwSensorMgr sensor_mgr(sensor_mgr_params);

Board board{
    .bmp390 = bmp390,
    .imu = imu,
    .timebase = timebase,
    .i2c = i2c,
    .sensor_mgr = sensor_mgr,
};

bool bsp_init()
{
    // Initialize system clock first
    bool ret = clock.init();

    // Enable peripheral clocks:
    // - GPIOA (BNO055 RST on PA0)
    // - GPIOB (I2C1 SCL/SDA on PB8/PB9)
    // - I2C1
    // - TIM3 (Timebase)
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN | RCC_AHB4ENR_GPIOBEN;
    RCC->APB1LENR |= RCC_APB1LENR_I2C1EN | RCC_APB1LENR_TIM3EN;

    // Initialize I2C pins, reset pin, and I2C peripheral
    ret = ret && sda.init();
    ret = ret && scl.init();
    ret = ret && rst.init();
    ret = ret && i2c.init();

    // Reset sequence for BNO055
    ret = ret && rst.set(false);
    LBR::Utils::DelayMs(10);
    ret = ret && rst.set(true);
    LBR::Utils::DelayMs(650);

    // Initialize sensors
    ret = ret && bmp390.init();
    imu.init();

    // Configure NVIC for TIM3 timebase
    NVIC_DisableIRQ(TIM3_IRQn);
    NVIC_ClearPendingIRQ(TIM3_IRQn);
    NVIC_SetPriority(TIM3_IRQn, 5U);

    // Initialize timebase driver
    ret = ret && timebase.init();

    // Unmask TIM3 interrupt
    NVIC_ClearPendingIRQ(TIM3_IRQn);
    NVIC_EnableIRQ(TIM3_IRQn);

    return ret;
}

Board& get_board()
{
    return board;
}

extern "C" void TIM3_IRQHandler(void)
{
    timebase.handle_irq();
}

}   // namespace LBR