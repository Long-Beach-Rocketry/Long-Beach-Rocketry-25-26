/**
 * @file    st_spi_factory.cc
 * @author  Bex
 * @brief   Factory for creating valid SPI objects with GPIO settings
 * @version 0.2
 * @date    2025-10-05
 */

#include "st_spi_factory.h"

namespace LBR {
namespace Stml4 {

/**
 * @brief Create an SPI object with given instance, GPIO config, and SPI settings
 * - Configures SCK, MOSI, and MISO pins with alternate function
 * - Applies user-provided SPI settings
 */
HwSpi CreateSpi(SPI_TypeDef* spi_instance,
                const SpiGpioConfig& gpio_config,
                const StSpiSettings& spi_settings) {
    // Construct SPI object with instance + settings
    HwSpi spi(spi_instance, spi_settings);

    // Initialize GPIO pins for SPI
    HwGpio sck_pin(gpio_config.sck_settings.port, gpio_config.sck_settings.pin_num);
    sck_pin.Init(CreateSpiPinSettings(gpio_config.sck_settings.af));

    HwGpio miso_pin(gpio_config.miso_settings.port, gpio_config.miso_settings.pin_num);
    miso_pin.Init(CreateSpiPinSettings(gpio_config.miso_settings.af));

    HwGpio mosi_pin(gpio_config.mosi_settings.port, gpio_config.mosi_settings.pin_num);
    mosi_pin.Init(CreateSpiPinSettings(gpio_config.mosi_settings.af));

    return spi;
}

/**
 * @brief Validate the SPI object
 * @return true if initialized successfully, false otherwise
 */
bool ValidateSpi(HwSpi& spi) {
    return (spi.Init() == SpiStatus::OK);
}

/**
 * @brief High-level factory function to get a validated SPI object
 * - Creates SPI object with GPIO config and settings
 * - Validates initialization
 * - Returns SPI object if valid, otherwise a default invalid SPI object
 */
HwSpi GetSpi(SPI_TypeDef* spi_instance,
             const SpiGpioConfig& gpio_config,
             const StSpiSettings& spi_settings) {
    HwSpi spi = CreateSpi(spi_instance, gpio_config, spi_settings);
    if (ValidateSpi(spi)) {
        return spi; // return the valid object
    } else {
        // Return a default invalid SPI object
        return HwSpi(nullptr, StSpiSettings{});
    }
}

}
} 
