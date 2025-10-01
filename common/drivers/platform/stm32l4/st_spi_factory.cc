#include "st_spi_factory.h"

/**
 * @brief Attempt to initialize an SPI object
 * 
 * @param instance 
 * @param gpio_settings 
 * @param gpio_names 
 * @param cfg 
 * @return HwSpi The SPI object we wish to use before being validated
 */
HwSpi CreateSpi(SPI_TypeDef* instance, SpiGpioSettings& gpio_settings, SpiGpioNames& gpio_names, SpiCrSettings& cfg) {
    // Instantiate new SPI object
    HwSpi spi{instance, cfg};

    // Create a struct with sck, miso, and mosi pin ojects and populate them with the desired settings, port, and pin number
    SpiPins spi_pins;
    spi_pins.sck{CreateSpiPinSettings(gpio_settings.sck_settings.af), gpio_names.sck_name.pin, gpio_names.sck_name.port};
    spi_pins.miso{CreateSpiPinSettings(gpio_settings.miso_settings.af), gpio_names.miso_name.pin, gpio_names.miso_name.port};
    spi_pins.mosi{CreateSpiPinSettings(gpio_settings.mosi_settings.af), gpio_names.mosi_name.pin, gpio_names.mosi_name.port};

    // Assign that SPI pin struct to the SPI object member variable pins
    spi.SetPins(spi_pins);

    // Change the control register 1 and 2 settings for the SPI object
    spi.SpiConfigSettings(cfg);

    return spi;
}

/**
 * @brief Checks if the SPI object created has good values in it
 * 
 * @param spi 
 * @return true 
 * @return false 
 */
bool ValidateSpi(HwSpi& spi) {
    // TODO: Check if the SPI object has valid values inside of it
}

/**
 * @brief Returns the SPI object if everything was initialized correctly
 * 
 * @param instance 
 * @param gpio_settings 
 * @param gpio_names 
 * @param cfg 
 * @return HwSpi The SPI object after validation (empty SPI object with false initialization flag if failed)
 */
HwSpi GetSpi(SPI_TypeDef* instance, SpiGpioSettings& gpio_settings, SpiGpioNames& gpio_names, SpiCrSettings& cfg) {
    HwSpi spi = CreateSpi(instance, gpio_settings, gpio_names, cfg);

    if(!ValidateSpi(spi)) {
        // TODO: Implement ERROR Flag here
        return HwSpi(); // Return empty HwSpi object with false initialization variable
    }

    return spi;
}