/**
 * @file    st_spi.cc
 * @author  Bex Saw
 * @brief   Bare-metal SPI driver for STM32L4
 * @version 0.2
 * @date    2025-10-05
 */

#include "st_spi.h"

namespace LBR {
    namespace Stml4 {
        SpiStatus HwSpi::Init()
        {
            if (!instance_)
            return SpiStatus::INIT_ERR;
            
            // Disable SPI before config
            instance_->CR1 &= ~SPI_CR1_SPE;

            // Full duplex
            instance_->CR1 |= SPI_CR1_MSTR;
            instance_->CR1 &= ~SPI_CR1_RXONLY;

            // (NSS) 
            instance_->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
            
            // Apply all settings
            if (!SetSpiBaudRate(settings_.baudRate))     
                return SpiStatus::INIT_ERR;
            if (!SetSpiBusMode(settings_.busMode))       
                return SpiStatus::INIT_ERR;
            if (!SetDataSize(settings_.dataSize))        
                return SpiStatus::INIT_ERR;
            if (!SetBitOrder(settings_.bitOrder))        
                return SpiStatus::INIT_ERR;
            if (!SetRxThreshold(settings_.rxThreshold))  
                return SpiStatus::INIT_ERR;

            // Enable SPI
            instance_->CR1 |= SPI_CR1_SPE;

            initialized_ = true;
            return SpiStatus::OK;
}

/**
 * @brief Blocking send operation
 * @note SetSpiBaudRate, SetSpiBusMode, SetDataSize, SetBitOrder, SetRxThreshold
 * must be called before this function 
 */

bool HwSpi::SetSpiBaudRate(SpiBaudRate baudRate)
{
    if (!instance_)
        return false;

    // Clear divider bits, then set new value
    instance_->CR1 &= ~SPI_CR1_BR_Msk;
    instance_->CR1 |= (static_cast<uint32_t>(baudRate) << SPI_CR1_BR_Pos);

    return true;
}


bool HwSpi::SetSpiBusMode(SpiBusMode mode)
{
    if (!instance_)
        return false;

    // Clear CPOL and CPHA
    instance_->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);

    // Extract bit patterns directly from enum value
    uint8_t mode_val = static_cast<uint8_t>(mode);

    uint32_t cpol_bit = (mode_val >> 1) & 0x01;  // high bit
    uint32_t cpha_bit = mode_val & 0x01;         // low bit

    instance_->CR1 |= (cpol_bit * SPI_CR1_CPOL);
    instance_->CR1 |= (cpha_bit * SPI_CR1_CPHA);

    return true;
}


bool HwSpi::SetDataSize(SpiDataSize size)
{
    if (!instance_)
        return false;

    instance_->CR2 &= ~SPI_CR2_DS_Msk;

    if (size == SpiDataSize::DATA_8BIT)
        instance_->CR2 |= (0x7U << SPI_CR2_DS_Pos);
    else
        instance_->CR2 |= (0xFU << SPI_CR2_DS_Pos);

    return true;
}


bool HwSpi::SetBitOrder(SpiBitOrder order)
{
    if (!instance_)
        return false;

    if (order == SpiBitOrder::MSB_FIRST)
        instance_->CR1 &= ~SPI_CR1_LSBFIRST;
    else
        instance_->CR1 |= SPI_CR1_LSBFIRST;

    return true;
}


bool HwSpi::SetRxThreshold(SpiRxThreshold th)
{
    if (!instance_)
        return false;

    if (th == SpiRxThreshold::RX_8BIT)
        instance_->CR2 |= SPI_CR2_FRXTH;
    else
        instance_->CR2 &= ~SPI_CR2_FRXTH;

    return true;
}

} 
} 
