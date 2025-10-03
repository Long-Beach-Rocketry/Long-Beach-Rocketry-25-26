/**
 * @file st_spi.cc
 * @author Bex
 * @brief SPI driver class implementation for STM32L4
 * @version 0.1
 * @date 2025-10-01
 */

#include "st_spi.h"

namespace LBR {
namespace Stml4 {

/**
 * @brief Initialize and configure the SPI peripheral
 * - Sets master mode
 * - Enables full-duplex
 * - Applies baud rate, mode, data size, bit order, RX threshold
 * - Enables software NSS
 * - Turns SPI on
 */

SpiStatus HwSpi::Init() {
    if (!instance_) return SpiStatus::INIT_ERR;

    // Master mode
    instance_->CR1 |= SPI_CR1_MSTR;

    // Full-duplex (clear RXONLY to allow TX/RX together)
    instance_->CR1 &= ~SPI_CR1_RXONLY;

    // TODO: Fix the CR1 and CR2 settings based on current config baud rate

    // Software slave management (keeps NSS high unless toggled)
    instance_->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);

    // Enable SPI peripheral
    instance_->CR1 |= SPI_CR1_SPE;

    initialized_ = true;
    return SpiStatus::OK;
}

/**
 * @brief Configure baud rate (clock speed divider)
 * @param baudrate Divider factor (F_PCLK / 2, 4, 8, …)
 */
bool HwSpi::SetSpiBaudRate(SpiBaudRate baudrate) {
    if (!instance_) return false;
    instance_->CR1 &= ~SPI_CR1_BR;  // Clear old divider
    instance_->CR1 |= (static_cast<uint32_t>(baudrate) << SPI_CR1_BR_Pos);
    return true;
}

/**
 * @brief Configure clock polarity (CPOL) and phase (CPHA)
 * @param mode Bus mode (0..3) that maps directly to CPOL/CPHA bits
 */
bool HwSpi::SetSpiBusMode(SpiBusMode mode) {
    if (!instance_) return false;
    instance_->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);   // Clear old mode
    instance_->CR1 |= static_cast<uint32_t>(mode);      // Apply mode bits
    return true;
}

/**
 * @brief Configure frame size (8-bit or 16-bit)
 * @param size Frame size enum mapped to DS bits
 */
bool HwSpi::SetDataSize(SpiDataSize size) {
    if (!instance_) return false;
    instance_->CR2 &= ~SPI_CR2_DS;                      // Clear DS
    instance_->CR2 |= (static_cast<uint32_t>(size) << SPI_CR2_DS_Pos);
    return true;
}

/**
 * @brief Configure bit order (MSB-first or LSB-first)
 * @param order Which bit to shift out first
 */
bool HwSpi::SetBitOrder(SpiBitOrder order) {
    if (!instance_) return false;
    if (order == SpiBitOrder::MSB_FIRST)
        instance_->CR1 &= ~SPI_CR1_LSBFIRST;
    else
        instance_->CR1 |=  SPI_CR1_LSBFIRST;
    return true;
}

/**
 * @brief Configure RX FIFO threshold (8-bit or 16-bit trigger level)
 * @param th Threshold enum
 */
bool HwSpi::SetRxThreshold(SpiRxThreshold th) {
    if (!instance_) return false;
    if (th == SpiRxThreshold::RX_8BIT)
        instance_->CR2 |=  SPI_CR2_FRXTH;
    else
        instance_->CR2 &= ~SPI_CR2_FRXTH;
    return true;
}

} // namespace Stml4
} // namespace LBR
