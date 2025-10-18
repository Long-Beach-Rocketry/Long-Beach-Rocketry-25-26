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

    // Init SPI gpio pins
    sck_pin.init();
    miso_pin.init();
    mosi_pin.init();

    // Init SPI instance
    spi.Init();

    return spi;
}
