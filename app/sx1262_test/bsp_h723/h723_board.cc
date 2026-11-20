#include "board.h"

#include "gpio_cs.h"
#include "st_gpio.h"
#include "st_spi.h"
#include "stm32h723xx.h"

using namespace LBR::Stmh7;

namespace LBR
{
namespace
{

// SX1262 bench wiring:
// PD14 -> NSS
// PA5  -> SCK
// PA6  -> MISO
// PB5  -> MOSI
// PD15 -> NRESET
// PD12 -> BUSY
const StGpioSettings kChipSelectSettings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                                         GpioOspeed::VERY_HIGH,
                                         GpioPupd::NO_PULL, 0};
const StGpioSettings kResetSettings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                                    GpioOspeed::LOW, GpioPupd::NO_PULL, 0};
const StGpioSettings kBusySettings{GpioMode::INPUT, GpioOtype::PUSH_PULL,
                                   GpioOspeed::LOW, GpioPupd::NO_PULL, 0};
const StGpioSettings kSpiDataSettings{GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                                      GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL,
                                      5};
const StGpioSettings kSpiClockSettings{GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                                       GpioOspeed::VERY_HIGH,
                                       GpioPupd::PULL_DOWN, 5};

const StGpioParams kChipSelectParams{kChipSelectSettings, 14, GPIOD};
const StGpioParams kResetParams{kResetSettings, 15, GPIOD};
const StGpioParams kBusyParams{kBusySettings, 12, GPIOD};
const StGpioParams kClockParams{kSpiClockSettings, 5, GPIOA};
const StGpioParams kMisoParams{kSpiDataSettings, 6, GPIOA};
const StGpioParams kMosiParams{kSpiDataSettings, 5, GPIOB};

HwGpio chip_select_pin{kChipSelectParams};
HwGpio reset_pin{kResetParams};
HwGpio busy_pin{kBusyParams};
HwGpio clock_pin{kClockParams};
HwGpio miso_pin{kMisoParams};
HwGpio mosi_pin{kMosiParams};

StSpiSettings spi_settings{SpiBaudRate::FPCLK_16, SpiBusMode::MODE1,
                           SpiBitOrder::MSB, SpiDataSize::DSIZE8,
                           SpiRxThreshold::FIFO_1};
HwSpi spi{SPI1, spi_settings};
GpioChipSelect chip_select{chip_select_pin};

Board board{
    .cs = chip_select,
    .spi = spi,
    .reset = reset_pin,
    .busy = busy_pin,
};

}  // namespace

extern "C" void IncDelayTicks();

bool bsp_init()
{
    RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
    (void)RCC->APB4ENR;

    PWR->CR3 = PWR_CR3_LDOEN;
    (void)PWR->CR3;
    PWR->D3CR = (PWR->D3CR & ~PWR_D3CR_VOS) | (3u << PWR_D3CR_VOS_Pos);
    (void)PWR->D3CR;
    while ((PWR->D3CR & PWR_D3CR_VOSRDY) == 0)
    {
    }

    RCC->AHB4ENR |=
        RCC_AHB4ENR_GPIOAEN | RCC_AHB4ENR_GPIOBEN | RCC_AHB4ENR_GPIODEN;
    (void)RCC->AHB4ENR;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    (void)RCC->APB2ENR;

    RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;

    RCC->D1CCIPR &= ~RCC_D1CCIPR_CKPERSEL;
    RCC->D2CCIP1R = (RCC->D2CCIP1R & ~RCC_D2CCIP1R_SPI123SEL_Msk) |
                    (4u << RCC_D2CCIP1R_SPI123SEL_Pos);

    bool ret = true;
    ret = ret && clock_pin.init();
    ret = ret && miso_pin.init();
    ret = ret && mosi_pin.init();
    ret = ret && chip_select_pin.init();
    ret = ret && reset_pin.init();
    ret = ret && busy_pin.init();
    ret = ret && spi.init();

    chip_select.cs_disable();
    reset_pin.set(1);

    // Configure SysTick for 1ms interrupts
    if (SysTick_Config(SystemCoreClock / 1000u) != 0u)
    {
        return false;
    }
    return ret;
}

Board& get_board()
{
    return board;
}

extern "C" void SysTick_Handler()
{
    IncDelayTicks();
}

}  // namespace LBR