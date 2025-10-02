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
LBR::Stml4::HwSpi CreateSpi(SPI_TypeDef *instance, SpiCrSettings &cfg) {
  // Instantiate new SPI object
  LBR::Stml4::HwSpi spi{instance, cfg};

  // Change the control register 1 and 2 settings for the SPI object
  spi.SpiConfigSettings(cfg);

  return spi;
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
LBR::Stml4::HwSpi GetSpi(SPI_TypeDef *instance, SpiCrSettings &cfg) {
  LBR::Stml4::HwSpi spi = LBR::Stml4::CreateSpi(instance, cfg);

  if (!ValidateSpi(spi)) {
    return LBR::Stml4::HwSpi(); // Return empty HwSpi object with false
                                // initialization variable
  }

  return spi;
}