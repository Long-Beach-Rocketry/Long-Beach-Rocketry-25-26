#include "stm32l4xx.h"
#include "st_spi.h"
#include "st_spi_defs.h"

using namespace LBR::Stml4;

int main() {
    //1. Enable clock for SPI1 and GPIOA (assuming PA5=SCK, PA6=MISO, PA7=MOSI)
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enable SPI1 clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // Enable GPIOA clock

    //2. Define SPI Settings
    StSpiSettings spiSettings{
        SpiBaudRate::FPCLK_8, // Baud rate divider
        SpiBusMode::MODE1,    // CPOL=0, CPHA=1
        SpiDataSize::DATA8_BIT,  // 8-bit data size
        SpiBitOrder::MSB_FIRST, // MSB first
        SpiRxThreshold::RX_8BIT // RX threshold
    };

    //3. Create SPI object for SPI1
    HwSpi spi(SPI1, settings);

    //4. Initialize SPI
    if(spi.Init() != SpiStatus::OK) {
        // Handle initialization error
        while(1);
    }

    //5. Prepare data to send
    uint8_t txData[] = {0xAA, 0x55};
    spi.Send(txData, 2, 1000); // Send data with 1 second timeout 

    //6. Test: Read 2 bytes (requires loopback: PA7 back to back with PA6)
    uint8_t rxData[2] = {0};
    spi.Read(rxData, 2, 1000); // Read data with 1 second timeout

    //7. Debug: verify rx_data == tx_data if loopback is connected
    while(1) {
    __NOP(); //Idle loop
    }
}
