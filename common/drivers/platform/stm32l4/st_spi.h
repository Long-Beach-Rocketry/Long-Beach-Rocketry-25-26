/**
 * @file st_spi.h
 * @author Kent Hong
 * @brief SPI Driver library for STM32l4xx
 * @version 0.1
 * @date 2025-09-28
 * 
 * @note How to use:
 * 
 *          Declare an SPI object with the current SPI peripheral you are using (ex. SPI1, SPI2, or SPI3) and SpiPinMap structs 
 *          with the desired pin mapping in st_spi_pins.h to populate the nss, sck, miso, and mosi member variables.
 * 
 * @example If we want to use SPI1 with GPIOA pins:
 *              
 *          LBR::Stml4::SPI spi_obj{SPI1, SPI1_NSS[0], SPI1_SCK[0], SPI1_MOSI[0], SPI1_MISO[0]};
 * 
 *          This will set nss_pin_, sck_pin_, miso_pin_, and mosi_pin_ member variables to the desired pins we want to use on the STM32L476RG
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include "st_gpio.h"
#include "st_spi_defs.h"
#include "st_spi_pins.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Helper function to create Gpio settings for SPI functionality
 * 
 * @note Created as an inline function to avoid function call overhead
 */
namespace LBR::Stml4 {
    inline StGpioSettings CreateSpiPinSettings(uint8_t af) {
        return StGpioSettings{GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL, GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, af};
    }
}

namespace LBR {
    namespace Stml4 {
        class SPI {
            public:

                // ?: we might not even need to create an nss_pin object since we are using software slave management to enable the chip select
                explicit SPI(SPI_Typedef* instance, const SpiPinMap& sck, const SpiPinMap& miso, const SpiPinMap& mosi);
                explicit SPI(SPI_Typedef* instance, const SpiPinMap& nss, const SpiPinMap& sck, const SpiPinMap& miso, const SpiPinMap& mosi);
                SpiStatus Init();

                // TODO: implement read, write, and transfer for SPI
                SpiStatus Read();
                SpiStatus Write(std::uint16_t output_data);
                SpiStatus Transfer(std::uint16_t output_data);

                bool SetSpiBaudRate(SpiBaudRate baudrate);
                bool SetSpiBusMode(SpiBusMode mode);
                bool SetSpiDataSize(SpiDataSize datasize);
                void SpiConfigSettings(const SpiCrSettings& cfg);
                
            private:
                // Member variables
                SPI_Typedef* instance_;
                SpiCrSettings settings_;
                HwGpio mosi_pin_;
                HwGpio miso_pin_;
                HwGpio sck_pin_;
                HwGpio nss_pin_;
        };
    }
}