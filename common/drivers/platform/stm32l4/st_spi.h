/**
 * @file    st_spi.h
 * @author  Bex Saw
 * @brief   Bare-metal STM32L4 SPI driver (implements Spi interface)
 * @version 0.2
 * @date    2025-10-05
 *
 * @note Usage:
 *   LBR::Stml4::HwSpi spi(SPI1, settings);
 *   spi.Init();
 *   spi.Send(tx_buf, len, 1000);
 */

#pragma once

#include "stm32l4xx.h"
#include "spi.h"
#include "st_spi_settings.h"
#include <cstdint>
#include <cstddef>

namespace LBR {
    namespace Stml4 {
        class HwSpi : public Spi {
            public:
                // Construct with SPI instance and initial settings
                    HwSpi(SPI_TypeDef* instance, const StSpiSettings& settings)
                        : instance_(instance), settings_(settings) {}

                // Initialize SPI and apply settings
                    SpiStatus Init();

                // Basic blocking operations
                    bool Send(const std::uint8_t* data,
                            std::size_t size,
                            std::uint32_t timeout);

                    bool Read(std::uint8_t* data,
                            std::size_t size,
                            std::uint32_t timeout);

                    bool Transfer(const std::uint8_t* tx_data,
                            std::uint8_t* rx_data,
                            std::size_t size,
                            std::uint32_t timeout);

                // Required by Spi interface (according to spi.h)
                    bool Read() override   { return true; }
                    bool Write() override  { return true; }
                    bool Transfer() override { return true; }

            private:
                // Hardware instance and config
                SPI_TypeDef*   instance_      { nullptr };
                StSpiSettings  settings_      { };
                bool initialized_             { false };

                // Internal helpers
                bool SetSpiBaudRate(SpiBaudRate baudrate);
                bool SetSpiBusMode(SpiBusMode mode);
                bool SetDataSize(SpiDataSize size);
                bool SetBitOrder(SpiBitOrder order);
                bool SetRxThreshold(SpiRxThreshold th);
            };

}   
}   
