#include "board.h"
#include "gpio_cs.h"
#include "st_gpio.h"
#include "st_spi.h"
#include "stm32h723xx.h"

// Pin Configurations:
// PD14 -> SPI1_CS   (Chip Select - Active Low)
// PA5  -> SPI1_SCK  (Serial Clock - Configured with PULL_DOWN for SPI Mode 0)
// PA6  -> SPI1_MISO (Master In Slave Out - Alternate Function 5)
// PB5  -> SPI1_MOSI (Master Out Slave In - Alternate Function 5)

using namespace LBR::Stmh7;
namespace LBR
{

StGpioSettings cs_gpio_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                                GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, 0};
StGpioParams cs_params{cs_gpio_settings, 14, GPIOD};
HwGpio cs_gpio{cs_params};

// Make SPI Register Config Settings
Stmh7::StSpiSettings spi_settings{
    Stmh7::SpiBaudRate::FPCLK_16, Stmh7::SpiBusMode::MODE1,
    Stmh7::SpiBitOrder::MSB, Stmh7::SpiDataSize::DSIZE8,
    Stmh7::SpiRxThreshold::FIFO_1};
HwSpi spi1{SPI1, spi_settings};

// Make GPIO Register Config Settings
Stmh7::StGpioSettings spi_data_settings{
    Stmh7::GpioMode::ALT_FUNC, Stmh7::GpioOtype::PUSH_PULL,
    Stmh7::GpioOspeed::VERY_HIGH, Stmh7::GpioPupd::NO_PULL, 5};

Stmh7::StGpioSettings sck_settings{
    Stmh7::GpioMode::ALT_FUNC, Stmh7::GpioOtype::PUSH_PULL,
    Stmh7::GpioOspeed::VERY_HIGH, Stmh7::GpioPupd::PULL_DOWN, 5};

Stmh7::StGpioParams sck_params{sck_settings, 5, GPIOA};
Stmh7::StGpioParams miso_params{spi_data_settings, 6, GPIOA};
Stmh7::StGpioParams mosi_params{spi_data_settings, 5, GPIOB};

// Make GPIO Spi pins
HwGpio sck{sck_params};
HwGpio miso{miso_params};
HwGpio mosi{mosi_params};

// Create Chip Select Object to manual toggle cs pin
GpioChipSelect chip_select{cs_gpio};

Board board{.cs = chip_select, .spi = spi1};

// Initialize SPI BSP
bool bsp_init()
{
    // Reference status
    bool result = true;

    // Enable clock gate to power configuration block
    RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
    (void)RCC->APB4ENR;

    // Configure power supplies for Direct LDO mode matching option bytes
    PWR->CR3 = PWR_CR3_LDOEN;
    (void)PWR->CR3;

    // Set core voltage scaling to VOS1 high-performance mode
    PWR->D3CR = (PWR->D3CR & ~PWR_D3CR_VOS) | (3 << PWR_D3CR_VOS_Pos);
    (void)PWR->D3CR;

    // Wait until physical power rails stabilize
    while (!(PWR->D3CR & PWR_D3CR_VOSRDY))
    {
        // Spin here until hardware power rails are fully stable
    }

    // Enable clock gates for GPIO Ports A, B, and D
    RCC->AHB4ENR |=
        (RCC_AHB4ENR_GPIOAEN | RCC_AHB4ENR_GPIOBEN | RCC_AHB4ENR_GPIODEN);
    (void)RCC->AHB4ENR;

    // Enable SPI1 clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    (void)RCC->APB2ENR;

    // Reset SPI1 peripheral to clear sticky hardware states
    RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;

    // Set peripheral root clock (per_ck) source to fallback HSI
    RCC->D1CCIPR &= ~RCC_D1CCIPR_CKPERSEL;

    // Route SPI1/2/3 kernel clock domain to per_ck
    RCC->D2CCIP1R = (RCC->D2CCIP1R & ~RCC_D2CCIP1R_SPI123SEL_Msk) |
                    (4 << RCC_D2CCIP1R_SPI123SEL_Pos);

    // Init SPI pins and CS pin
    result = result && sck.init();
    result = result && miso.init();
    result = result && mosi.init();
    result = result && cs_gpio.init();

    // Init SPI periph and check if it was successful
    result = result && spi1.init();

    // Disable chip select to put flash in standby
    chip_select.cs_disable();

    return result;
}

Board& get_board()
{
    return board;
}

}  // namespace LBR