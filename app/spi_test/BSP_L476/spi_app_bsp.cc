#include "spi_app_bsp.h"
#include "stm32l476xx.h"
#include "st_spi_module.h"

using namespace LBR::Stml4;

// Initialize SPI BSP
Spi& BSP_Init(SPI_TypeDef* spi_instance, GPIO_TypeDef* gpio_instance)
{
    (void)spi_instance;

    // Enable GPIOA clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Enable SPI clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    StSpiSettings spi_settings{SpiBaudRate::FPCLK_16,
                              SpiBusMode::MODE1,
                              SpiBitOrder::MSB,
                              SpiRxThreshold::FIFO_8bit};
    StGpioSettings gpio_settings{
        GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL, GpioOspeed::HIGH,
        GpioPupd::NO_PULL, 5};  // AF5 for SPI1

    StGpioParams sck_pin{gpio_settings, 5, GPIOA};   // PA5 - SCK
    StGpioParams miso_pin{gpio_settings, 6, GPIOA};
    StGpioParams mosi_pin{gpio_settings, 7, GPIOA};  // PA7 - MOSI
    SpiModule spi_module(SPI1, spi_settings, sck_pin, miso_pin, mosi_pin);

    static HwSpi spi = spi_module.CreateSpi();

    return spi;
}
