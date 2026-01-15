/**
 * @file st_spi.cc
 * @author Kent Hong
 * @brief SPI Driver class implementation
 * @date 2025-09-30
 *
 */

#include "st_spi.h"

namespace LBR
{
namespace Stml4
{

HwSpi::HwSpi(SPI_TypeDef* instance_, StSpiSettings& settings_)
    : instance{instance_}, settings{settings_}
{
}

bool HwSpi::Read(std::span<uint8_t> rx_data)
{

    // Check if SPI is already in communication
    if (instance->SR & SPI_SR_BSY)
    {
        return false;
    }

    for (auto& byte : rx_data)
    {
        // TODO: ADD a timeout

        // Wait until TX Buffer is empty before sending dummy byte to generate clock pulse
        while (!(instance->SR & SPI_SR_TXE))
        {
        }

        // Send dummy byte to DR to generate clock pulse
        *(volatile uint8_t*)&instance->DR = 0x00;

        // Wait until RX buffer has enough data to be read
        // TODO: ADD a timeout

        while (!(instance->SR & SPI_SR_RXNE))
        {
        }

        // Read data from RX buffer
        byte = *(volatile uint8_t*)&instance->DR;
    }

    // Wait until transmission is complete
    while (instance->SR & SPI_SR_BSY)
    {
    }

    return true;
}

bool HwSpi::Write(std::span<uint8_t> tx_data)
{

    // Check if SPI is already in communication
    if (instance->SR & SPI_SR_BSY)
    {
        return false;
    }

    for (const auto& byte : tx_data)
    {
        // Wait until TX Buffer is empty
        while (!(instance->SR & SPI_SR_TXE))
        {
        }

        /*
         * Write to SPI Data Register (DR).
         * Automatically starts clock once data is written to the DR.
         */
        *(volatile uint8_t*)&instance->DR = byte;

        /*
         * Wait for RX buffer to be filled.
         * Even if only writing, RXNE flag has to be cleared before proceeding to the next byte write.
         */
        while (!(instance->SR & SPI_SR_RXNE))
        {
        }

        /*
         * Clear RXNE flag by performing a dummy read in the RX Buffer.
         * Dereferencing the data reg performs a read. Type casting to void explicitly discards the unused value.
         */
        (void)(*(volatile uint8_t*)&instance->DR);
    }

    // Wait until transmission is complete
    while (instance->SR & SPI_SR_BSY)
    {
    }

    return true;
}

bool HwSpi::SeqTransfer(std::span<uint8_t> tx_data, std::span<uint8_t> rx_data)
{
    // Check if SPI is enabled
    if (!(instance->CR1 & SPI_CR1_SPE))
    {
        return false;
    }

    // Check if SPI is already in communication
    if (instance->SR & SPI_SR_BSY)
    {
        return false;
    }

    /*
     * First send all tx bytes and clear the RXNE flag for each transmitted
     * byte (we don't use these intermediate bytes for flash commands).
     */
    for (const auto& byte : tx_data)
    {
        // Wait until TX Buffer is empty
        while (!(instance->SR & SPI_SR_TXE))
        {
        }

        // Write next tx byte to start clocking
        *(volatile uint8_t*)&instance->DR = byte;

        // Wait for RX buffer to be filled (data received for this transfer)
        while (!(instance->SR & SPI_SR_RXNE))
        {
        }

        // Read and discard intermediate RX byte to clear RXNE
        (void)(*(volatile uint8_t*)&instance->DR);
    }

    /*
     * Now generate clock pulses by sending dummy bytes to read the expected
     * response from the slave into rx_data[0..rx_len-1].
     */
    for (auto& byte : rx_data)
    {
        // Wait until TX Buffer is empty before sending dummy byte
        while (!(instance->SR & SPI_SR_TXE))
        {
        }

        // Send dummy byte to generate clock for slave to shift out data
        *(volatile uint8_t*)&instance->DR = 0x00;

        // Wait until RX buffer has data
        while (!(instance->SR & SPI_SR_RXNE))
        {
        }

        // Read data from RX buffer into rx_data
        byte = *(volatile uint8_t*)&instance->DR;
    }

    // Wait until transmission is complete
    while (instance->SR & SPI_SR_BSY)
    {
    }

    return true;
}

bool HwSpi::Init()
{
    // TODO: Runtime validation of enum values (will change to compile time checks in the future and maybe make a private function for these checks)
    if (static_cast<uint8_t>(settings.baudrate) > 7)
    {
        return false;
    }
    if (static_cast<uint8_t>(settings.busmode) > 3)
    {
        return false;
    }
    if (static_cast<uint8_t>(settings.order) > 1)
    {
        return false;
    }
    if (static_cast<uint8_t>(settings.threshold) > 1)
    {
        return false;
    }

    /* 
     * Set to Master Mode (Keep in master mode unless we want our STM32l4xx to be
     * used as a slave device). Master Mode sets our STM32l4xx to act as the
     * master device to initiate/end SPI communication and drive the clock signal. 
     */
    instance->CR1 |= SPI_CR1_MSTR;

    /* 
     * Enable Full-Duplex Mode (Can send and receive data simultaneously through
     * MOSI and MISO) 
     */
    instance->CR1 &= ~(SPI_CR1_RXONLY);

    // Configure SPI sck Baudrate
    SetReg(&instance->CR1, uint32_t(settings.baudrate), 3, 3);

    // Configure the SPI Bus Mode
    SetReg(&instance->CR1, uint32_t(settings.busmode), 0, 2);

    // Configure Bit order
    SetReg(&instance->CR1, uint32_t(settings.order), 7, 1);

    /* 
     * Determine FIFO reception threshold to see how many bits in RX Buffer
     * triggers an RXNE event
     */
    SetReg(&instance->CR2, uint32_t(settings.threshold), 12, 1);

    // Configure the SPI data size to 8 bits
    instance->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2);

    // Use software slave management and toggle SSI = 1 to pull NSS to high
    // Toggle SSI = 0 when we want to select the slave device for data transfer
    instance->CR1 |= SPI_CR1_SSM;
    instance->CR1 |= SPI_CR1_SSI;

    // Enable SPI peripheral
    instance->CR1 |= SPI_CR1_SPE;

    return true;
}
}  // namespace Stml4
}  // namespace LBR