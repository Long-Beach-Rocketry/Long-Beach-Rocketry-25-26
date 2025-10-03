/**
 * @file    st_spi.h
 * @author  Bex
 * @brief   STM32L4 SPI Driver (C++ only, implements Spi interface)
 * @version 0.1
 * @date    2025-10-01
 *
 * @note Usage:
 *   LBR::Stml4::HwSpi spi_obj(SPI1, settings);
 *   spi_obj.Init();
 *   spi_obj.Send(tx_buf, len, 1000);
 */

#pragma once

#include "stm32l4xx.h"
#include "spi.h"
#include "st_spi_defs.h"
#include <cstdint>
#include <cstddef>

namespace LBR {
    namespace Stml4 {
        class HwSpi : public Spi {
        public: 
            HwSpi() = default;

            // Construct with peripheral + initial settings
            HwSpi(SPI_TypeDef* instance, const SpiCrSettings& settings)
                : instance_(instance), settings_(settings) {}

            // Configure registers and enable SPI
            SpiStatus Init();

            // Blocking buffer operations
            bool Send(const std::uint8_t* data, std::size_t size, std::uint32_t timeout);
            bool Read(std::uint8_t* data, std::size_t size, std::uint32_t timeout);
            bool Transfer(const std::uint8_t* tx_data, std::uint8_t* rx_data,
                          std::size_t size, std::uint32_t timeout);

            // Update settings (before Init or while disabled)
            void ApplySettings(const SpiCrSettings& settings) { settings_ = settings; }

            // Interface overrides (can just call the buffer ops)
            bool Read() override;
            bool Write() override;
            bool Transfer() override;

        private:
            SPI_TypeDef* instance_{nullptr}; // SPI1, SPI2, SPI3...
            SpiCrSettings settings_{};       // Current configuration
            bool initialized_{false};        // State flag

            // Internal helpers
            bool SetSpiBaudRate(SpiBaudRate baudrate);
            bool SetSpiBusMode(SpiBusMode mode);
            bool SetDataSize(SpiDataSize size);
            bool SetBitOrder(SpiBitOrder order);
            bool SetRxThreshold(SpiRxThreshold th);
            bool SpiConfigSettings(const SpiCrSettings& cfg);
        };
    } 
}
