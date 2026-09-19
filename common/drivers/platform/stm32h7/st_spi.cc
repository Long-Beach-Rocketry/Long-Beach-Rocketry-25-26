/**
 * @file st_spi.cc
 * @author Alex Pulido
 * @brief SPI Driver class implementation
 * @date 2026-05-22
 *
 */

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
    size_t total_bytes = rx_data.size();
    if (total_bytes == 0)
        return false;

    // Disable peripheral to unlock configuration registers
    instance->CR1 &= ~SPI_CR1_SPE;
    (void)instance->CR1;

    // Flush any leftover bytes in the receive queue
    while (instance->SR & SPI_SR_RXP)
    {
        volatile uint32_t dummy = instance->RXDR;
        (void)dummy;
    }

    // Configure total transfer count and master/ssm options
    instance->CR2 = (instance->CR2 & ~SPI_CR2_TSIZE_Msk) |
                    (total_bytes << SPI_CR2_TSIZE_Pos);
    instance->CFG2 |= (SPI_CFG2_MASTER | SPI_CFG2_SSM);
    (void)instance->CFG2;

    // Re-enable peripheral and trigger clock generation
    instance->CR1 |= (SPI_CR1_SPE | SPI_CR1_SSI);
    (void)instance->CR1;
    instance->CR1 |= SPI_CR1_CSTART;

    size_t tx_count = 0;
    size_t rx_count = 0;

    // Full-duplex loop pushing clocks out to pull data back
    while (rx_count < total_bytes)
    {
        if ((instance->SR & SPI_SR_TXP) && (tx_count < total_bytes))
        {
            *((volatile uint8_t*)&instance->TXDR) = 0x00;
            tx_count++;
        }

        if (instance->SR & SPI_SR_RXP)
        {
            rx_data[rx_count++] = *((volatile uint8_t*)&instance->RXDR);
        }
    }

    // Wait for the transfer block to finish shifting
    while (!(instance->SR & SPI_SR_EOT))
    {
    }

    // Clear completed event tracking flags
    instance->IFCR |= (SPI_IFCR_EOTC | SPI_IFCR_TXTFC);

    return true;
}

bool HwSpi::write(std::span<uint8_t> tx_data)
{
    size_t total_bytes = tx_data.size();
    if (total_bytes == 0)
        return false;

    // Disable peripheral to unlock configuration registers
    instance->CR1 &= ~SPI_CR1_SPE;
    (void)instance->CR1;

    // Configure total transfer count and master/ssm options
    instance->CR2 = (instance->CR2 & ~SPI_CR2_TSIZE_Msk) |
                    (total_bytes << SPI_CR2_TSIZE_Pos);
    instance->CFG2 |= (SPI_CFG2_MASTER | SPI_CFG2_SSM);
    (void)instance->CFG2;

    // Re-enable peripheral and trigger master transmission
    instance->CR1 |= (SPI_CR1_SPE | SPI_CR1_SSI);
    (void)instance->CR1;
    instance->CR1 |= SPI_CR1_CSTART;

    size_t tx_count = 0;
    size_t rx_count = 0;

    // Full-duplex loop driving lines while draining incoming dummy bytes
    while (tx_count < total_bytes || rx_count < total_bytes)
    {
        if ((instance->SR & SPI_SR_TXP) && (tx_count < total_bytes))
        {
            *((volatile uint8_t*)&instance->TXDR) = tx_data[tx_count++];
        }

        if (instance->SR & SPI_SR_RXP)
        {
            volatile uint8_t dummy = *((volatile uint8_t*)&instance->RXDR);
            (void)dummy;
            rx_count++;
        }
    }

    // Wait for the transfer block to finish shifting
    while (!(instance->SR & SPI_SR_EOT))
    {
    }

    // Clear completed event tracking flags
    instance->IFCR |= (SPI_IFCR_EOTC | SPI_IFCR_TXTFC);

    return true;
}

bool HwSpi::seq_transfer(std::span<uint8_t> tx_data, std::span<uint8_t> rx_data)
{
    size_t total_bytes = tx_data.size();
    if (total_bytes == 0)
        return false;

    // Disable peripheral to unlock configuration registers
    instance->CR1 &= ~SPI_CR1_SPE;
    (void)instance->CR1;

    // Flush any leftover bytes in the receive queue
    while (instance->SR & SPI_SR_RXP)
    {
        volatile uint32_t dummy = instance->RXDR;
        (void)dummy;
    }

    // Configure total transfer count and master/ssm options
    instance->CR2 = (instance->CR2 & ~SPI_CR2_TSIZE_Msk) |
                    (total_bytes << SPI_CR2_TSIZE_Pos);
    instance->CFG2 |= (SPI_CFG2_MASTER | SPI_CFG2_SSM);
    (void)instance->CFG2;

    // Re-enable peripheral and start concurrent transfer
    instance->CR1 |= (SPI_CR1_SPE | SPI_CR1_SSI);
    (void)instance->CR1;
    instance->CR1 |= SPI_CR1_CSTART;

    size_t tx_count = 0;
    size_t rx_count = 0;

    // Match both indices simultaneously over the wire layout
    while (rx_count < total_bytes)
    {
        if ((instance->SR & SPI_SR_TXP) && (tx_count < total_bytes))
        {
            *((volatile uint8_t*)&instance->TXDR) = tx_data[tx_count++];
        }

        if (instance->SR & SPI_SR_RXP)
        {
            rx_data[rx_count++] = *((volatile uint8_t*)&instance->RXDR);
        }
    }

    // Wait for the transfer block to finish shifting
    while (!(instance->SR & SPI_SR_EOT))
    {
    }

    // Clear completed event tracking flags
    instance->IFCR |= (SPI_IFCR_EOTC | SPI_IFCR_TXTFC);

    return true;
}

bool HwSpi::init()
{
    // Validate configuration options
    if (static_cast<uint8_t>(settings.baudrate) > 7)
        return false;
    if (static_cast<uint8_t>(settings.busmode) > 3)
        return false;
    if (static_cast<uint8_t>(settings.order) > 1)
        return false;
    if (static_cast<uint8_t>(settings.datasize) > 31 ||
        static_cast<uint8_t>(settings.datasize) < 3)
        return false;

    // Explicitly drop setup lock parameters, and disable I2S mode
    instance->CR1 &= ~SPI_CR1_SPE;
    instance->I2SCFGR &= ~SPI_I2SCFGR_I2SMOD;

    // Set to Master Mode
    instance->CFG2 |= SPI_CFG2_MASTER;

    // Enable full duplex mode
    instance->CFG2 &= ~(SPI_CFG2_COMM);

    // Write parameters into register structures
    SetReg(&instance->CFG1, uint32_t(settings.baudrate), 28, 3);
    SetReg(&instance->CFG2, uint32_t(settings.busmode), 24, 2);
    SetReg(&instance->CFG2, uint32_t(settings.order), 23, 1);
    SetReg(&instance->CFG1, uint32_t(settings.datasize), 0, 5);
    SetReg(&instance->CFG1, uint32_t(settings.threshold), 5, 4);

    // Apply software slave state handling overrides
    instance->CFG2 |= SPI_CFG2_SSM;
    instance->CR1 |= SPI_CR1_SSI;

    instance->CR2 = 0;
    instance->IFCR = 0xFFFFFFFF;

    // Enable SPI peripheral
    instance->CR1 |= SPI_CR1_SPE;

    return true;
}

}  // namespace Stmh7
}  // namespace LBR