#include "board.h"
#include "gpio_cs.h"
#include "st_gpio.h"
#include "st_spi.h"
#include "stm32h723xx.h"

using namespace LBR::Stmh7;
namespace LBR
{

StGpioSettings cs_gpio_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                                GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, 0};
StGpioParams cs_params{cs_gpio_settings, 4, GPIOA};
HwGpio cs_gpio{cs_params};

// Make SPI Register Config Settings
Stmh7::StSpiSettings spi_settings{
    Stmh7::SpiBaudRate::FPCLK_2, Stmh7::SpiBusMode::MODE1,
    Stmh7::SpiBitOrder::MSB, Stmh7::SpiRxThreshold::FIFO_8bit};
HwSpi spi1{SPI1, spi_settings};

// Make GPIO Register Config Settings
Stmh7::StGpioSettings gpio_settings{
    Stmh7::GpioMode::ALT_FUNC, Stmh7::GpioOtype::PUSH_PULL,
    Stmh7::GpioOspeed::VERY_HIGH, Stmh7::GpioPupd::NO_PULL, 5};
Stmh7::StGpioParams sck_params{gpio_settings, 5, GPIOA};
Stmh7::StGpioParams miso_params{gpio_settings, 6, GPIOA};
Stmh7::StGpioParams mosi_params{gpio_settings, 7, GPIOA};

// Make GPIO Spi pins
Stmh7::HwGpio sck{sck_params};
Stmh7::HwGpio miso{miso_params};
Stmh7::HwGpio mosi{mosi_params};

// Create Chip Select Object to manual toggle cs pin
GpioChipSelect chip_select{cs_gpio};

Board board{.cs = chip_select, .spi1 = spi1};

// Initialize SPI BSP
bool bsp_init()
{
    // Reference status
    bool result = true;

    // Enable GPIOA clock
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;

    // Enable SPI clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Init SPI periph and check if it was successful
    result = result && spi1.init();

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
