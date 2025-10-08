/**
 * @file st_spi_module.cc
 * @author Kent Hong
 * @brief SPI Module Class implementation
 * @date 2025-10-07
 * 
 */

#include "st_spi_module.h"

LBR::Stml4::SpiModule::SpiModule(SPI_TypeDef* instance_, StSpiSettings& cfg_,
                                 StGpioParams& sck_p, StGpioParams& miso_p,
                                 StGpioParams& mosi_p)
    : instance{instance_},
      cfg{cfg_},
      sck_pin{sck_p},
      miso_pin{miso_p},
      mosi_pin{mosi_p}
{
}

/**
 * @brief Attempt to initialize an SPI object with desired control register
 * settings
 * @return HwSpi The SPI object we wish to use before being validated
 */
LBR::Stml4::HwSpi LBR::Stml4::SpiModule::CreateSpi()
{
    // Instantiate new SPI object
    LBR::Stml4::HwSpi spi{instance, cfg};

    return spi;
}

/**
 * @brief Returns the SPI object if everything was initialized correctly
 * @return HwSpi The SPI object with configured control register settings after
 * validation (empty SPI object with false initialization variable if failed)
 */
LBR::Stml4::HwSpi LBR::Stml4::SpiModule::GetSpi()
{
    LBR::Stml4::HwSpi spi = CreateSpi();

    bool status = ValidateSpi(spi);

    if (status)
    {
        spi.Init();
    }
    // TODO: Add else logic to false status

    return spi;
}