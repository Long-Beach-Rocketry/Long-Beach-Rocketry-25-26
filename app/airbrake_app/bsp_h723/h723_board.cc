#include "board.h"
#include "delay.h"
#include "st_gpio.h"
#include "st_i2c.h"
#include "st_sensor_manager.h"
#include "st_sysclk.h"
#include "st_timebase.h"

static constexpr uint32_t kSysclkHz{64'000'000};
// For 64 MHz HSI, I2C timing value from STM32CubeMX
static constexpr uint32_t kI2cTimingR{0x20303E5D};
// Default APB1 timer input frequency on HSI (64 MHz)
static constexpr uint32_t kTim3ClkHz{64'000'000};

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

// Clock config (Sysclk = 64 MHz HSI, AHB = 64 MHz, APB1..4 = 64 MHz)
Stmh7::ClockParams clock_params{
    Stmh7::Source::HSI64_MHZ,     kSysclkHz,
    Stmh7::D1cprePrescaler::DIV1, Stmh7::AhbPrescaler::DIV1,
    Stmh7::Apb1Prescaler::DIV1,   Stmh7::Apb2Prescaler::DIV1,
    Stmh7::Apb3Prescaler::DIV1,   Stmh7::Apb4Prescaler::DIV1};
Stmh7::HwClock clock{clock_params};

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

// TIM3 Timebase (16-bit, APB1)
const Stmh7::StTimebaseParams timebase_params{TIM3, TIM3_IRQn, kTim3ClkHz,
                                              true};
HwTimebase timebase(timebase_params);

// Sensors managed by the sensor manager
const StSensorMgrSensors sensor_mgr_sensors{
    .bno055 = &imu,
    .bmp390 = &baro,
};

const StSensorMgrParams sensor_mgr_params{
    .timebase = timebase,
    .sensors = sensor_mgr_sensors,
    .ekf = nullptr,  // TODO: no EKF instance yet, add once implemented
};

HwSensorMgr sensor_mgr(sensor_mgr_params);

}  // namespace Stmh7

Board board{.clock = Stmh7::clock,
            .timebase = timebase,
            .bno055 = Stmh7::imu,
            .bmp390 = Stmh7::baro,
            .sensor_mgr = Stmh7::sensor_mgr};

bool board_init()
{
    bool ret = true;

    // Enable peripheral clocks
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN | RCC_AHB4ENR_GPIOBEN;
    RCC->APB1LENR |= RCC_APB1LENR_I2C1EN | RCC_APB1LENR_TIM3EN;

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

    // Initialize timebase driver
    ret = ret && Stmh7::timebase.init();

    // Unmask TIM3 interrupt
    NVIC_ClearPendingIRQ(TIM3_IRQn);
    NVIC_EnableIRQ(TIM3_IRQn);

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

extern "C" void TIM3_IRQHandler(void)
{
    Stmh7::timebase.handle_irq();
}

}  // namespace LBR