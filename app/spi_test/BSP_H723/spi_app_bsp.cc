#include "board.h"
#include "gpio_cs.h"
#include "st_gpio.h"
#include "st_spi.h"
#include "stm32h723xx.h"

using namespace LBR::Stmh7;

// PB9 SPI2_NSS, PC2 SPI2_MISO, PB15 SPI2_MOSI, PB13 SPI2_SCK

namespace LBR
{

// Make Chip Select Settings
StGpioSettings cs_gpio_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                                GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, 0};
StGpioParams cs_params{cs_gpio_settings, 9, GPIOB};  // PB9
HwGpio cs_gpio{cs_params};

// Make SPI Register Config Settings
Stmh7::StSpiSettings spi_settings{
    Stmh7::SpiBaudRate::FPCLK_16, Stmh7::SpiBusMode::MODE1,
    Stmh7::SpiBitOrder::MSB, Stmh7::SpiDataSize::DSIZE8,
    Stmh7::SpiRxThreshold::FIFO_8};
Stmh7::HwSpi spi2{SPI2, spi_settings};

// Make GPIO Register Config Settings
Stmh7::StGpioSettings gpio_settings{
    Stmh7::GpioMode::ALT_FUNC, Stmh7::GpioOtype::PUSH_PULL,
    Stmh7::GpioOspeed::VERY_HIGH, Stmh7::GpioPupd::NO_PULL, 5};
Stmh7::StGpioParams sck_params{gpio_settings, 13, GPIOB};
Stmh7::StGpioParams miso_params{gpio_settings, 2, GPIOC};
Stmh7::StGpioParams mosi_params{gpio_settings, 15, GPIOB};

// Make GPIO Spi pins
Stmh7::HwGpio sck{sck_params};
Stmh7::HwGpio miso{miso_params};
Stmh7::HwGpio mosi{mosi_params};

// Create Chip Select Object to manual toggle cs pin
GpioChipSelect chip_select{cs_gpio};

Board board{.cs = chip_select, .spi2 = spi2};

// Initialize SPI BSP
bool bsp_init()
{
    // Reference status
    bool result = true;

    // Enable GPIOB, GPIOC clocks
    RCC->AHB4ENR |=
        (RCC_AHB4ENR_GPIOBEN | RCC_AHB4ENR_GPIOCEN);

    // Enable the SPI2 clock
    RCC->APB1LENR |= RCC_APB1LENR_SPI2EN;

    // Dummy read to ensure clock is enabled before accessing SPI2
    (void)RCC->APB1LENR; 

    // Enable SYSCFG clock 
    RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;

    // Dummy read to ensure clock propagates
    (void)RCC->APB4ENR; 

    // Connect PC2 to PC2_C analog switch for digital use
    SYSCFG->PMCR |= SYSCFG_PMCR_PC2SO;

    // Init SPI periph and check if it was successful
    result = result && spi2.init();

    // Init Spi pins
    result = result && sck.init();
    result = result && miso.init();
    result = result && mosi.init();

    // Init CS pin
    result = result && cs_gpio.init();
    chip_select.cs_enable();

    return result;
}

Board& get_board()
{
    return board;
}
}  // namespace LBR