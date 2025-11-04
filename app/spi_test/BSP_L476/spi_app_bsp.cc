#include "board.h"
#include "st_gpio.h"
#include "st_spi.h"
#include "st_spi_module.h"
#include "stm32l476xx.h"

namespace LBR
{
// TODO: Initialize Board struct in board.h to fill in values for the 4 GPIOs and 1 SPI

// Initialize SPI BSP
Spi& BSP_Init(SPI_TypeDef* spi_instance, GPIO_TypeDef* gpio_instance)
{

    // Enable GPIOA clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Enable SPI clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Make SPI Register Config Settings
    Stml4::StSpiSettings spi_settings{
        Stml4::SpiBaudRate::FPCLK_2, Stml4::SpiBusMode::MODE1,
        Stml4::SpiBitOrder::MSB, Stml4::SpiRxThreshold::FIFO_8bit};

    // Make GPIO Register Config Settings
    Stml4::StGpioSettings gpio_settings{
        Stml4::GpioMode::ALT_FUNC, Stml4::GpioOtype::PUSH_PULL,
        Stml4::GpioOspeed::VERY_HIGH, Stml4::GpioPupd::NO_PULL, 5};
    Stml4::StGpioParams clk_params{gpio_settings, 5, gpio_instance};
    Stml4::StGpioParams miso_params{gpio_settings, 6, gpio_instance};
    Stml4::StGpioParams mosi_params{gpio_settings, 7, gpio_instance};

    // Create ready to use SPI object based on the SPI & GPIO Register settings from earlier
    Stml4::SpiModule spi_mod(spi_instance, spi_settings, clk_params,
                             miso_params, mosi_params);

    static Stml4::HwSpi spi = spi_mod.CreateSpi();

    return spi;
}
}  // namespace LBR
