#include "board.h"
#include "gpio_cs.h"
#include "st_gpio.h"
#include "st_spi.h"
#include "stm32l476xx.h"

using namespace LBR::Stml4;
namespace LBR
{

StGpioSettings cs_gpio_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                                GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, 0};
StGpioParams cs_params{cs_gpio_settings, 4, GPIOA};
HwGpio cs_gpio{cs_params};

// Make SPI Register Config Settings
Stml4::StSpiSettings spi_settings{
    Stml4::SpiBaudRate::FPCLK_2, Stml4::SpiBusMode::MODE1,
    Stml4::SpiBitOrder::MSB, Stml4::SpiRxThreshold::FIFO_8bit};
HwSpi spi1{SPI1, spi_settings};

// Make GPIO Register Config Settings
Stml4::StGpioSettings gpio_settings{
    Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::PUSH_PULL,
    Stml4::GpioOspeed::VERY_HIGH, Stml4::GpioPupd::NO_PULL, 5};
Stml4::StGpioParams sck_params{gpio_settings, 5, GPIOA};
Stml4::StGpioParams miso_params{gpio_settings, 6, GPIOA};
Stml4::StGpioParams mosi_params{gpio_settings, 7, GPIOA};

// Make GPIO Spi pins
Stml4::HwGpio sck{sck_params};
Stml4::HwGpio miso{miso_params};
Stml4::HwGpio mosi{mosi_params};

// Create Chip Select Object to manual toggle cs pin
GpioChipSelect chip_select{cs_gpio};

Board board{.cs = chip_select, .spi1 = spi1};

// Initialize SPI BSP
bool BSP_Init()
{
    // Reference status
    bool result = true;

    // Enable GPIOA clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Enable SPI clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Init SPI periph and check if it was successful
    result = result && spi1.Init();

    // Init Spi pins
    result = result && sck.init();
    result = result && miso.init();
    result = result && mosi.init();

    // Init CS pin
    result = result && cs_gpio.init();
    chip_select.ChipSelectEnable();

    return result;
}

Board& Get_Board()
{
    return board;
}
}  // namespace LBR
