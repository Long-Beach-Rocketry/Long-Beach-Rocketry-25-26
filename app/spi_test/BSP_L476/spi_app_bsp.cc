#include "st_gpio.h"
#include "st_spi.h"
#include "st_spi_module.h"
#include "stm32l476xx.h"

using namespace LBR::Stml4;
using namespace LBR;

// Define SPI configuration
static StSpiSettings spi_cfg = {.baudrate = SpiBaudRate::FPCLK_16,
                                .busmode = SpiBusMode::MODE1,
                                .order = SpiBitOrder::MSB,
                                .threshold = SpiRxThreshold::FIFO_8bit};

// Create SPI1 instance
static HwSpi spi1(SPI1, spi_cfg);

// Initialize SPI BSP
Spi& BSP_Init(SPI_TypeDef* spi_instance, GPIO_TypeDef* gpio_instance)
{
    (void)spi_instance;  // avoid unused parameter warning

    // Enable GPIOA clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Enable SPI clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Make SPI Register Config Settings
    StSpiSettings spi_settings{SpiBaudRate::FPCLK_2, SpiBusMode::MODE1,
                               SpiBitOrder::MSB, SpiRxThreshold::FIFO_8bit};

    // Make GPIO Register Config Settings
    StGpioSettings gpio_settings{GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL,
                                 GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, 5};
    StGpioParams clk_params{gpio_settings, 5, gpio_instance};
    StGpioParams miso_params{gpio_settings, 6, gpio_instance};
    StGpioParams mosi_params{gpio_settings, 7, gpio_instance};

    // Create ready to use SPI object based on the SPI & GPIO Register settings from earlier
    SpiModule spi_mod(spi_instance, spi_settings, clk_params, miso_params,
                      mosi_params);

    static HwSpi spi = spi_mod.CreateSpi();

    return spi;
}
