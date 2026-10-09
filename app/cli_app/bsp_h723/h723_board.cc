#include <array>
#include "board.h"
#include "st_gpio.h"
#include "st_i2c.h"
#include "st_sysclk.h"
#include "st_usart.h"

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
 * PB0 - Led
 * PB8 - I2c SCL
 * PB9 - I2c SDA
 * PB14 - Led
 * PD8 - Uart tx
 * PD9 - Uart rx
 * PE1 - Led
 */

// Create Clock object (Sysclk = 65 MHz, HCLK = 32.5 MHz, PCLK1 = PCLK2 = PCLK3 = PCLK4 = 16.25 MHz)
ClockParams clock_params{Source::HSE8_MHZ_BYPASS, kSysclkHz,
                         D1cprePrescaler::DIV1,   AhbPrescaler::DIV2,
                         Apb1Prescaler::DIV2,     Apb2Prescaler::DIV2,
                         Apb3Prescaler::DIV2,     Apb4Prescaler::DIV2};
HwClock clock{clock_params};

// Use ST-Link VCP pins for USART3 on H723 boards (COM port output)
StGpioSettings usart_settings{GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                              GpioOspeed::LOW, GpioPupd::NO_PULL, 7};
StGpioParams usart_tx_params{usart_settings, 8, GPIOD};
StGpioParams usart_rx_params{usart_settings, 9, GPIOD};
HwGpio usart_tx{usart_tx_params};
HwGpio usart_rx{usart_rx_params};

// Create USART3 object
StUsartParams usart_params{USART3, kSysclkHz, kBaudRate};
StUsart usart{usart_params};

// Set up LED pins
StGpioSettings ld_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                           GpioOspeed::LOW, GpioPupd::NO_PULL, 0};
StGpioParams ld1_params{ld_settings, 0, GPIOB};
StGpioParams ld2_params{ld_settings, 1, GPIOE};
StGpioParams ld3_params{ld_settings, 14, GPIOB};

HwGpio ld1{ld1_params};
HwGpio ld2{ld2_params};
HwGpio ld3{ld3_params};

// Configure I2c
StI2cParams i2c_params{I2C1, kI2cTimingR};

HwI2c i2c(i2c_params);

// // Set up BARO pins (SCL PB8, SDA PB9)
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

// // Create BNO055 IMU object
Bno055 imu(static_cast<LBR::I2c&>(i2c), Bno055::ADDR_PRIMARY);

// Create Barometer object
Bmp390Params baro_params{i2c, 0x76};
Bmp390 baro{baro_params};

}  // namespace Stmh7

Board board{
    .usart = Stmh7::usart,
    .clock = Stmh7::clock,
    .led1 = Stmh7::ld1,
    .led2 = Stmh7::ld2,
    .led3 = Stmh7::ld3,
    .bno055 = Stmh7::imu,
    // .bmp390 = Stmh7::baro
};

bool board_init()
{
    bool ret = true;

    // Enable peripheral clocks
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN | RCC_AHB4ENR_GPIOBEN |
                    RCC_AHB4ENR_GPIODEN | RCC_AHB4ENR_GPIOEEN;
    RCC->APB1LENR |= RCC_APB1LENR_USART3EN | RCC_APB1LENR_I2C1EN;

    // Initialize USART pins and USART
    ret &= Stmh7::ld1.init();
    ret &= Stmh7::ld2.init();
    ret &= Stmh7::ld3.init();

    ret &= Stmh7::usart_tx.init();
    ret &= Stmh7::usart_rx.init();
    ret &= Stmh7::usart.init();

    // Configure clock tree for this test.
    bool clock_ok = Stmh7::clock.init();
    ret &= clock_ok;

    // Retune BRR using the actual post-init APB1 frequency.
    if (clock_ok)
    {
        const uint32_t apb1_hz = Stmh7::clock.get_clock_frequencies().apb1;
        USART_TypeDef* usart_addr = Stmh7::usart.get_addr();

        // Reconfigure BRR after clock switch and explicitly keep TX enabled.
        usart_addr->CR1 &= ~USART_CR1_UE;
        usart_addr->BRR = apb1_hz / kBaudRate;
        usart_addr->CR1 |= USART_CR1_TE | USART_CR1_RE;

        usart_addr->CR1 |= USART_CR1_RXNEIE_RXFNEIE;
        usart_addr->CR1 |= USART_CR1_UE;
    }
    NVIC_EnableIRQ(USART3_IRQn);

    ret &= Stmh7::sda.init();
    ret &= Stmh7::scl.init();

    ret &= Stmh7::i2c.init();

    // ret &= Stmh7::baro.init();

    ret &= Stmh7::rst.init();
    ret &= Stmh7::rst.set(false);
    Utils::DelayMs(10);
    ret &= Stmh7::rst.set(true);
    Utils::DelayMs(650);
    Stmh7::imu.init();

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

extern "C" void USART3_IRQHandler(void)
{
    if (Stmh7::usart.get_addr()->ISR & USART_ISR_RXNE_RXFNE)
    {
        if (board.usart.receive(rxb))
        {
            // received 1 byte, echo back

            update_flag = cli.take_input(rxb);
            if (update_flag)
            {
                std::array<uint8_t, 4> newLine{"\r\n\n"};
                board.usart.send(newLine);
            }
        }
    }
}
}  // namespace LBR