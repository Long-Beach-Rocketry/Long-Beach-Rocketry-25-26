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
            if(!instance_) 
                return SpiStatus::INIT_ERR;
            
            // Disable SPI before configuration
            instance_->CR1 &= ~SPI_CR1_SPE;

            // Master and full-duplex
            instance_->CR1 |= SPI_CR1_MSTR 
            instance_->CR1 &= ~SPI_CR1_RXONLY;

            // Slave select management (comeback later)
            instance_->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;

            //Apply settings
            if(!SetSpiBaudRate(settings_.baudRate)) 
                return SpiStatus::INIT_ERR;
            if(!SetSpiBusMode(settings_.busMode)) 
                return SpiStatus::INIT_ERR;
            if(!SetSpiDataSize(settings_.dataSize))
                return SpiStatus::INIT_ERR;
            if(!SetSpiBitOrder(settings_.bitOrder))
                return SpiStatus::INIT_ERR;
            if(!SetSpiRxThreshold(settings_.rxThreshold))
                return SpiStatus::INIT_ERR;

            // Enable SPI
            instance_->CR1 |= SPI_CR1_SPE;

            initiaized_ = true;
            return SpiStatus::OK;
            }

        /**
         * @brief Congigure the SPI baud rate
         * @param baudRate Desired baud rate setting
         */

        bool HwSpi::SetSpiBaudRate(SpiBaudRate baudRate) {
            if(!instance_) 
                return false;

            // Clear divider bits
            instance_->CR1 &= ~SPI_CR1_BR;

            // Set new baud rate
            instance_->CR1 |= static_cast<uint32_t>(baudRate) << SPI_CR1_BR_Pos;
            return true;
            }

        /**
         * @brief Configure CPOL and CPHA
         */

        bool HwSpi::SetSpiBusMode(SpiBusMode busMode) {
            if(!instance_) 
                return false;

            // Clear CPOL and CPHA bits
            instance_->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);

            // Set new bus mode
            instance_->CR1 |= (static_cast<uint32_t>(busMode) << SPI_CR1_CPOL_Pos);
            instance_->CR1 |= (static_cast<uint32_t>(busMode) & 0x01) << SPI_CR1_CPHA_Pos;
            return true;
            }
        /**
        * @brief Configure data frame size
        */

        bool HwSpi::SetDataSize(SpiDataSize size) {
            if (!instance_) 
                return false;
            instance_->CR2 &= ~SPI_CR2_DS; // clear

            if (size == SpiDataSize::DATA_8BIT)
                instance_->CR2 |= (0x7U << SPI_CR2_DS_Pos); // 8-bit frame
            else
                instance_->CR2 |= (0xFU << SPI_CR2_DS_Pos); // 16-bit frame
                return true;
            }

        /**
        * @brief Configure bit order
        */

        bool HwSpi::SetBitOrder(SpiBitOrder order) {
            if (!instance_) 
                return false;
            if (order == SpiBitOrder::MSB_FIRST)
                instance_->CR1 &= ~SPI_CR1_LSBFIRST;
            else
                instance_->CR1 |=  SPI_CR1_LSBFIRST;
            return true;
            }

        /**
        * @brief Configure RX FIFO threshold
        */
        bool HwSpi::SetRxThreshold(SpiRxThreshold th) {
            if (!instance_) return false;
            if (th == SpiRxThreshold::RX_8BIT)
                instance_->CR2 |=  SPI_CR2_FRXTH;
            else
                instance_->CR2 &= ~SPI_CR2_FRXTH;
            return true;
            }
        }
    }