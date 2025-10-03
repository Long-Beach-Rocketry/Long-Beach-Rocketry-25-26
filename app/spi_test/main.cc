/**
 * @file main.cc
 * @author Bex
 * @brief SPI test application for STM32L4 using HwSpi driver
 * @version 0.1
 * @date 2025-10-01
 */


#include "stm32l4xx.h"
#include "st_spi.h"
#include "st_spi_settings.h"

using namespace LBR::Stml4;

int main() {
    // Enable clock for SPI1 and GPIOA (PA5=SCK, PA6=MISO, PA7=MOSI)
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;   // Enable SPI1 clk
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;  // Enable GPIOA clk

    // Define SPI Settings
    StSpiSettings spiSettings{
        SpiBaudRate::FPCLK_8,   
        SpiBusMode::MODE2,      // CPOL=0, CPHA=1 (loopback test?)
        SpiDataSize::DATA_8BIT, 
        SpiBitOrder::MSB_FIRST, 
        SpiRxThreshold::RX_8BIT 
    };

    // Create SPI object for SPI1
    HwSpi spi(SPI1, spiSettings);

    // Initialize SPI
    if (spi.Init() != SpiStatus::OK) {
        // Check initialization error
        while (1);
    }

    // Prepare data to send
    uint8_t txData[] = {0xAA, 0x55};
    spi.Send(txData, 2, 1000); // Send 2 bytes 

    // Read back 2 bytes (requires hardware loopback PA7 <-> PA6)
    uint8_t rxData[2] = {0};
    spi.Read(rxData, 2, 1000);

    // Verify rxData matches txData if loopback is connected
    while (1) {
        __NOP(); //Main loop
    }
}
