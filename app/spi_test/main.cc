/**
 * @file main.cc
 * @brief Bare-metal SPI1 communication test on STM32L476.
 * @author Bex Saw
 */

#include "st_gpio.h"
#include "st_spi_module.h"
#include "stm32l476xx.h"

using namespace LBR::Stml4;

int main(void)
{
    // Enable peripheral clks

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;  // Enable GPIOA clk
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;   // Enable SPI1 clk

    // SPI configuration
    StSpiSettings cfg{SpiBaudRate::FPCLK_8,
                      SpiBusMode::MODE1,  // MODE1
                      SpiBitOrder::MSB, SpiRxThreshold::FIFO_8bit};

    // GPIO configuration for SPI1 pins (Subject to Change)
    StGpioParams sck_pin{
        {GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL, GpioOspeed::HIGH,
         GpioPupd::NO_PULL, 5},  // AF5
        5,
        GPIOA  // PA5 - SCK
    };

    StGpioParams miso_pin{
        {GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL, GpioOspeed::HIGH,
         GpioPupd::NO_PULL, 5},  // AF5
        6,
        GPIOA  // PA6 - MISO
    };

    StGpioParams mosi_pin{
        {GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL, GpioOspeed::HIGH,
         GpioPupd::NO_PULL, 5},  // AF5
        7,
        GPIOA  // PA7 - MOSI
    };

    // Init SPI Module
    SpiModule spi_module(SPI1, cfg, sck_pin, miso_pin, mosi_pin);

    // Create SPI driver
    HwSpi spi = spi_module.CreateSpi();

    // Initialize SPI Peripheral
    if (!spi.Init())
        while (1);  // Trap CPU if SPI init failed

    // Data buffers
    uint8_t txData[2] = {0xAB, 0xCD};
    uint8_t rxData[2] = {0x00, 0x00};

    // Perform full-duplex SPI transfer
    spi.Transfer(txData, rxData, 2);

    // Idle loop
    while (1);
}
