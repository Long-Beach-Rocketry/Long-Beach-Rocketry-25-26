/**
 * @file    st_spi_factory.h
 * @author  Bex Saw
 * @brief Factory for creating valid SPI objects with GPIO settings
 * @version 0.1
 * @date 2025-10-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

#include "st_spi.h"
#include "st_gpio.h"

/**
 * @note:
 * Factory pattern:
 * 1. Configures SPI GPIO pins (SCK, MISO, MOSI)
 * 2. Creates and validates a HwSPI object
 * 3. Provides a simple GetSpi() interface for BSP
 */

 namespace LBR {
    namespace Stml4 {

        /**
         * @brief Build a default GPIO settings struct for SPI pins
         * @param sck_pin GPIO pin for SCK
        
        */
       inline StGpioSettings CreateSpiPinSettings(uint8_t af) {
           return StGpioSettings{
               .mode = StGpioMode::ALTERNATE,
               .otype = StGpioOType::PUSHPULL,
               .pupd = StGpioPuPd::NOPUPD,
               .speed = StGpioSpeed::VERY_HIGH,
               .af = af
           };
       }

    struct GpioName {
        GPIO_TypeDef* port;
        std::uint8_t pin_num;
    };

    /**
     * @brief COllection of GPIO configs for SCK, MISO, MOSI pins.
     */

    struct SpiGpioConfig {
        GpioName sck_settings;
        GpioName miso_settings;
        GpioName mosi_settings;
    };

    /**
     * @brief Names (port and pin number) for SPI GPIO pins
     */
    struct SpiGpioNames {
        GpioName sck_name;
        GpioName miso_name;
        GpioName mosi_name;
    };

    HwSpi CreateSpi(SPI_TypeDef* spi_instance, const SpiGpioConfig& gpio_config, const StSpiSettings& spi_settings);


    //CHeck if a given SPI object is valid and initialized
    bool ValidateSpi(HwSpi& spi);

    HwSpi GetSpi(SPI_TypeDef* spi_instance, const SpiGpioConfig& gpio_config, const StSpiSettings& spi_settings);
    } 
 }
 