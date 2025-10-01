/**
 * @file st_spi_factory.h
 * @author Kent Hong
 * @brief SPI Factory that creates valid SPI objects for use in the BSP
 * @version 0.1
 * @date 2025-10-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once
#include "st_spi.h"
#include "st_gpio.h"
#include "st_spi_pins.h"

namespace LBR {
    namespace Stml4 {

        /**
        * @brief Helper function to create HwGpio objects for an SPI pin
        * 
        */
        inline StGpioSettings CreateSpiPinSettings(uint8_t af) {
            return StGpioSettings{GpioMode::ALT_FUNC, GpioOtype::PUSH_PULL, GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, af};
        }

        struct GpioName {
            GPIO_TypeDef* port;
            std::uint8_t pin_num;
        };

        struct SpiGpioSettings {
            StGpioSettings sck_settings;
            StGpioSettings miso_settings;
            StGpioSettings mosi_settings;
        };

        struct SpiGpioNames {
            GpioName sck_name;
            GpioName miso_name;
            GpioName mosi_name;
        };

        HwSpi CreateSpi(SPI_TypeDef* instance, SpiGpioSettings& gpio_settings, SpiGpioNames& gpio_names, SpiCrSettings& cfg);
        bool ValidateSpi(HwSpi& spi);
        HwSpi GetSpi(SPI_TypeDef* instance, SpiGpioSettings& gpio_settings, SpiGpioNames& gpio_names, SpiCrSettings& cfg);
    }
}