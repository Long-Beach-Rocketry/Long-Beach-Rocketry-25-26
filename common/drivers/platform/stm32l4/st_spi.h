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
 *              LBR::Stml4::SPI spi_obj{SPI1, SPI1_NSS[0], SPI1_SCK[0], SPI1_MOSI[0], SPI1_MISO[0]};
 * 
 *          This will set nss_pin_, sck_pin_, miso_pin_, and mosi_pin_ member variables to the desired pins we want to use on the STM32L476RG.
 * 
 *          We can then modify the SPI peripheral control registers before using Init() by populating a SpiCrSettings struct with our desired 
 *          settings from the enum classes in st_spi_defs.h and feeding it into the SpiConfigSettings() function:
 * 
 *              SpiCrSettings settings{SpiBaudRate::FPCLK_2, SpiBusMode::MODE1, SpiDataSize::EIGHT_BIT, SpiBitOrder::MSB, SpiRxThreshold::FIFO_8bit};
 *              spi_obj.SpiConfigSettings(settings);
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include "spi.h"
#include "st_gpio.h"
#include "st_spi_defs.h"
#include "st_spi_pins.h"
#include <stdint.h>
#include <stdbool.h>

/** 
 * @brief Store SPI sck, miso, and mosi pins in one location
 */
struct SpiPins {
    HwGpio sck;
    HwGpio miso;
    HwGpio mosi;
};

namespace LBR {
    namespace Stml4 {
        class HwSpi : public Spi {
            public:
                explicit HwSpi();
                explicit HwSpi(SPI_Typedef* instance_, SpiCrSettings& settings_);
            
                // Member Functions
                SpiStatus Init();
                // TODO: implement read, write, and transfer for SPI
                SpiStatus Read();
                SpiStatus Write(std::uint16_t output_data);
                SpiStatus Transfer(std::uint16_t output_data);
               
                // Setter and Getter
                SpiPins GetPins const();
                void SetPins(const SpiPins& spi_pins_);
                
            private:
                // Member variables
                SPI_Typedef* instance;
                SpiCrSettings settings;
                SpiPins spi_pins;
                bool initialized;

                // Private Member Functions
                bool SetSpiBaudRate(SpiBaudRate baudrate);
                bool SetSpiBusMode(SpiBusMode mode);
                bool SetSpiDataSize(SpiDataSize datasize);
                void SpiConfigSettings(const SpiCrSettings& cfg);
        };
    }
}