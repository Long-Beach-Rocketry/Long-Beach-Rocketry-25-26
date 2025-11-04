/**
 * @file st_spi_module.cc
 * @author Kent Hong
 * @brief SPI Module Class implementation
 * @date 2025-10-07
 * 
 */

#include "st_spi_module.h"

// TODO:
/* 
 * 1. SpiModule class takes in an existing SPI Object w/ instance_ and cfg_ already initialized as well as sck_p, miso_p, and mosi_p structs
 * 2. CreateSpi() checks if the SPI Object has good values and inits sck, miso, and mosi pins -> returns bool
 * NOTE: We need to figure out how to implement compile time checks for CreateSpi()
 */
 
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
