/**
 * @file st_spi_module.cc
 * @author Kent Hong
 * @brief SPI Module Class implementation
 * @date 2025-10-07
 * 
 */

#include "st_spi_module.h"

LBR::Stml4::SpiModule::SpiModule(SPI_TypeDef* instance_, StSpiSettings& cfg_,
                                 HwGpio& sck_pin_, HwGpio& cs_pin_,
                                 HwGpio& miso_pin_, HwGpio& mosi_pin_)
    : instance{instance_},
      cfg{cfg_},
      sck_pin{sck_pin_},
      cs_pin{cs_pin_},
      miso_pin{miso_pin_},
      mosi_pin{mosi_pin_}
{
}

/**
 * @brief Attempt to initialize an SPI object with desired control register
 * settings
 *
 * @param instance
 * @param cfg
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
 *
 * @param instance
 * @param cfg
 * @return HwSpi The SPI object with configured control register settings after
 * validation (empty SPI object with false initialization variable if failed)
 */
LBR::Stml4::HwSpi LBR::Stml4::SpiModule::GetSpi()
{
    LBR::Stml4::HwSpi spi = CreateSpi();

    bool status = ValidateSpi(spi);

    return spi;
}