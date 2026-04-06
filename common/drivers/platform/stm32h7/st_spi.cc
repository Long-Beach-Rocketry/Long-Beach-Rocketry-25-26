#include "st_spi.h"
#include <cstdint>

// We'll be using pins: PB3 for SPI1_SCK (serial clock), PG9 for SPI1_MISO, PD7 for SPI1_MOSI, PG10 for SPI1_NSS (the chip selector (CS))
// PB3 GPIO alt. func. is under AF5, PG9 GPIO alt. func. is under AF5, PD7 GPIO alt. func. is under AF5, PG10 GPIO alt. func. is under AF5
// For STM, it splits the AFRL (GPIO alternate function low register (GPIOx_AFRL)) into pins 0-7 and AFRH (GPIO alternate function high register (GPIOx_AFRH)) 
// into pins 8-15, so PB3 and PD7 are in AFRL since 3 and 7 are in 0-7 and PG9 and PG10 are in AFRH since 9 and 10 are in 8-15.
/* So to summarize:

PB3 → AFRL, write 0101 into the AFR3 field
PD7 → AFRL, write 0101 into the AFR7 field
PG9 → AFRH, write 0101 into the AFR9 field
PG10 → AFRH, write 0101 into the AFR10 field */

namespace LBR
{
namespace Stmh7
{

HwSpi::HwSpi(SPI_TypeDef* instance_, StSpiSettings& settings_)
    : instance{instance_}, settings{settings_}
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
            if (--timeout == 0) return false;
        }

        // Send dummy byte to TXDR to generate clock pulse for slave to shift out data
        *(volatile uint8_t*)&instance->TXDR = 0x00;

        // TODO: Replace with a proper timer-based timeout
        timeout = 100000;

        // Wait until RX FIFO has at least one complete data packet available
        while (!(instance->SR & SPI_SR_RXP))
        {
            if (--timeout == 0) return false;
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
    for (const auto& byte : tx_data)
    {
        // TODO: Replace with a proper timer-based timeout
        uint32_t timeout = 100000;

        // Wait until TX FIFO has space available before writing next byte
        while (!(instance->SR & SPI_SR_TXP))
        {
            if (--timeout == 0) return false;
        }

        // Write next byte to TXDR to start clocking it out to the slave
        *(volatile uint8_t*)&instance->TXDR = byte;

        // TODO: Replace with a proper timer-based timeout
        timeout = 100000;

        // Wait for RX FIFO to be filled (data received for this transfer)
        while (!(instance->SR & SPI_SR_RXP))
        {
            if (--timeout == 0) return false;
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

    for (const auto& byte : tx_data)
    {
        // TODO: Replace with a proper timer-based timeout
        uint32_t timeout = 100000;

        // Wait until TX FIFO has space available before writing next byte
        while (!(instance->SR & SPI_SR_TXP))
        {
            if (--timeout == 0) return false;
        }

        // Write next byte to TXDR to start clocking it out to the slave
        *(volatile uint8_t*)&instance->TXDR = byte;

        // TODO: Replace with a proper timer-based timeout
        timeout = 100000;

        // Wait for RX FIFO to be filled (data received for this transfer)
        while (!(instance->SR & SPI_SR_RXP))
        {
            if (--timeout == 0) return false;
        }

        /*
         * Clear RXP flag by performing a dummy read from RXDR.
         * Dereferencing RXDR performs a read. Casting to void explicitly discards the unused value.
         */
        (void)(*(volatile uint8_t*)&instance->RXDR);
    }

    for (auto& byte : rx_data)
    {
        // TODO: Replace with a proper timer-based timeout
        uint32_t timeout = 100000;

        // Wait until TX FIFO has space available before sending dummy byte to generate clock pulse
        while (!(instance->SR & SPI_SR_TXP))
        {
            if (--timeout == 0) return false;
        }

        // Send dummy byte to TXDR to generate clock pulse for slave to shift out data
        *(volatile uint8_t*)&instance->TXDR = 0x00;

        // TODO: Replace with a proper timer-based timeout
        timeout = 100000;

        // Wait until RX FIFO has at least one complete data packet available
        while (!(instance->SR & SPI_SR_RXP))
        {
            if (--timeout == 0) return false;
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
    if (static_cast<uint8_t>(settings.baudrate) > 7)   return false;
    if (static_cast<uint8_t>(settings.busmode) > 3)    return false;
    if (static_cast<uint8_t>(settings.order) > 1)      return false;
    if (static_cast<uint8_t>(settings.datasize) > 31)  return false;

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
    instance->CR1 |= SPI_CR1_SSI;   // keep NSS high by default

    // Enable serial peripheral clock
    instance->CR1 |= SPI_CR1_SPE;

    return true;
}

}
}