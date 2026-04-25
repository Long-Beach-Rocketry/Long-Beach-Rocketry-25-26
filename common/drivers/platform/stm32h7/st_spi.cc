#include "st_spi.h"
#include <cstdint>

namespace LBR
{
namespace Stmh7
{

HwSpi::HwSpi(SPI_TypeDef* instance, StSpiSettings& settings)
    : instance{instance}, settings{settings}
{
}

bool HwSpi::read(std::span<uint8_t> rx_data)
{
    for (auto& byte : rx_data)
    {
        // TODO: Replace with a proper timer-based timeout
        uint32_t timeout = 100000;

        // Wait until TX FIFO has space available before sending dummy byte to generate clock pulse
        while (!(instance->SR & SPI_SR_TXP))
        {
            if (--timeout == 0)
                return false;
        }

        // Send dummy byte to TXDR to generate clock pulse for slave to shift out data
        *(volatile uint8_t*)&instance->TXDR = 0x00;

        // TODO: Replace with a proper timer-based timeout
        timeout = 100000;

        // Wait until RX FIFO has at least one complete data packet available
        while (!(instance->SR & SPI_SR_RXP))
        {
            if (--timeout == 0)
                return false;
        }

        // Read received byte from RX FIFO into buffer
        byte = *(volatile uint8_t*)&instance->RXDR;
    }

    // Wait until full transfer is complete
    while (!(instance->SR & SPI_SR_EOT))
    {
    }

    // Clear EOT flag by writing to IFCR, required before next transfer
    instance->IFCR |= SPI_IFCR_EOTC;

    return true;
}

bool HwSpi::write(std::span<uint8_t> tx_data)
{
    // Set TSIZE to number of bytes to transfer
    instance->CR2 = tx_data.size();

    // Set CSTART to initiate master transfer before the loop
    instance->CR1 |= SPI_CR1_CSTART;

    for (const auto& byte : tx_data)
    {
        // TODO: Replace with a proper timer-based timeout

        // Wait until TX FIFO has space available before writing next byte
        while (!(instance->SR & SPI_SR_TXP))
        {
        }

        // Write next byte to TXDR to start clocking it out to the slave
        *(volatile uint8_t*)&instance->TXDR = byte;

        // TODO: Replace with a proper timer-based timeout

        // Wait for RX FIFO to be filled (data received for this transfer)
        while (!(instance->SR & SPI_SR_RXP))
        {
        }

        /*
         * Clear RXP flag by performing a dummy read from RXDR.
         * Dereferencing RXDR performs a read. Casting to void explicitly discards the unused value.
         */
        (void)(*(volatile uint8_t*)&instance->RXDR);
    }

    // Wait until full transfer is complete
    while (!(instance->SR & SPI_SR_EOT))
    {
    }

    // Clear EOT flag by writing to IFCR, required before next transfer
    instance->IFCR |= SPI_IFCR_EOTC;

    return true;
}

bool HwSpi::seq_transfer(std::span<uint8_t> tx_data, std::span<uint8_t> rx_data)
{
    // Check if SPI is enabled
    if (!(instance->CR1 & SPI_CR1_SPE))
    {
        return false;
    }

    // Set TSIZE in CR2 to total number of bytes to transfer (tx + rx phases),
    // required on H7 so the master knows how many frames to clock before stopping
    instance->CR2 = tx_data.size() + rx_data.size();

    // Set CSTART to initiate master transfer
    instance->CR1 |= SPI_CR1_CSTART;

    for (const auto& byte : tx_data)
    {
        // Wait until TX FIFO has space available before writing next byte
        while (!(instance->SR & SPI_SR_TXP))
        {
        }

        // Write next byte to TXDR to start clocking it out to the slave
        *(volatile uint8_t*)&instance->TXDR = byte;

        // Wait for RX FIFO to be filled (data received for this transfer)
        while (!(instance->SR & SPI_SR_RXP))
        {
        }

        /*
         * Clear RXP flag by performing a dummy read from RXDR.
         * Dereferencing RXDR performs a read. Casting to void explicitly discards the unused value.
         */
        (void)(*(volatile uint8_t*)&instance->RXDR);
    }

    for (auto& byte : rx_data)
    {
        // Wait until TX FIFO has space available before sending dummy byte to generate clock pulse
        while (!(instance->SR & SPI_SR_TXP))
        {
        }

        // Send dummy byte to TXDR to generate clock pulse for slave to shift out data
        *(volatile uint8_t*)&instance->TXDR = 0x00;

        // Wait until RX FIFO has at least one complete data packet available
        while (!(instance->SR & SPI_SR_RXP))
        {
        }

        // Read received byte from RX FIFO into buffer
        byte = *(volatile uint8_t*)&instance->RXDR;
    }

    // Wait until full transfer is complete
    while (!(instance->SR & SPI_SR_EOT))
    {
    }

    // Clear EOT flag by writing to IFCR, required before next transfer
    instance->IFCR |= SPI_IFCR_EOTC;

    return true;
}

bool HwSpi::init()
{
    // Validate settings
    if (static_cast<uint8_t>(settings.baudrate) > 7)
        return false;
    if (static_cast<uint8_t>(settings.busmode) > 3)
        return false;
    if (static_cast<uint8_t>(settings.order) > 1)
        return false;
    if (static_cast<uint8_t>(settings.datasize) > 31)
        return false;

    // Set to master mode
    instance->CFG2 |= SPI_CFG2_MASTER;

    // Enable full duplex mode
    instance->CFG2 &= ~(SPI_CFG2_COMM);

    // Configure SPI sck Baudrate
    SetReg(&instance->CFG1, uint32_t(settings.baudrate), 28, 3);

    // Configure the SPI Bus Mode
    SetReg(&instance->CFG2, uint32_t(settings.busmode), 24, 2);

    // Configure Bit order
    SetReg(&instance->CFG2, uint32_t(settings.order), 23, 1);

    // Configure SPI datasize
    SetReg(&instance->CFG1, uint32_t(settings.datasize), 0, 5);

    // FIFO reception threshold
    SetReg(&instance->CFG1, uint32_t(settings.threshold), 5, 4);

    instance->CFG2 |= SPI_CFG2_SSM;  // software slave management
    instance->CR1 |= SPI_CR1_SSI;    // keep NSS high by default

    // Clear MODF fault before enabling SPE clock
    instance->IFCR |= SPI_IFCR_MODFC;

    // Enable serial peripheral clock
    instance->CR1 |= SPI_CR1_SPE;

    return true;
}

}  // namespace Stmh7
}  // namespace LBR