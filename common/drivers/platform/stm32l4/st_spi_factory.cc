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
LBR::Stml4::HwSpi CreateSpi(SPI_TypeDef *instance,
                            LBR::Stml4::SpiGpioSettings &gpio_settings,
                            LBR::Stml4::SpiGpioNames &gpio_names,
                            SpiCrSettings &cfg) {
  // Instantiate new SPI object
  LBR::Stml4::HwSpi spi{instance, cfg};

  // Create a struct with sck, miso, and mosi pin ojects and populate them with
  // the desired settings, port, and pin number
  SpiPins spi_pins;
  spi_pins.sck{LBR::Stml4::CreateSpiPinSettings(gpio_settings.sck_settings.af),
               gpio_names.sck_name.pin_num, gpio_names.sck_name.port};
  spi_pins.miso{
      LBR::Stml4::CreateSpiPinSettings(gpio_settings.miso_settings.af),
      gpio_names.miso_name.pin_num, gpio_names.miso_name.port};
  spi_pins.mosi{
      LBR::Stml4::CreateSpiPinSettings(gpio_settings.mosi_settings.af),
      gpio_names.mosi_name.pin_num, gpio_names.mosi_name.port};

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
bool ValidateSpi(LBR::Stml4::HwSpi &spi) {
  // TODO: Check if the SPI object has valid values inside of it
}

/**
 * @brief Returns the SPI object if everything was initialized correctly
 *
 * @param instance
 * @param gpio_settings
 * @param gpio_names
 * @param cfg
 * @return HwSpi The SPI object after validation (empty SPI object with false
 * initialization flag if failed)
 */
LBR::Stml4::HwSpi GetSpi(SPI_TypeDef *instance,
                         LBR::Stml4::SpiGpioSettings &gpio_settings,
                         LBR::Stml4::SpiGpioNames &gpio_names,
                         SpiCrSettings &cfg) {
  LBR::Stml4::HwSpi spi = LBR::Stml4::CreateSpi(instance, gpio_settings, gpio_names, cfg);

  if (!ValidateSpi(spi)) {
    // TODO: Implement ERROR Flag here
    return LBR::Stml4::HwSpi(); // Return empty HwSpi object with false
                                // initialization variable
  }

  return spi;
}