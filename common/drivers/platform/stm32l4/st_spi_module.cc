/**
 * @file st_spi_module.cc
 * @author Bex Saw
 * @brief SPI Module implementation for STM32L4 bare-metal environment.
 * @date 2025-10-07
 */

#include "st_spi_module.h"
#include "stm32l4xx.h"

namespace LBR::Stml4
{

SpiModule::SpiModule(SPI_TypeDef* instance_, const StSpiSettings& cfg_,
                     const StGpioParams& sck_p, const StGpioParams& miso_p,
                     const StGpioParams& mosi_p)
    : instance_{instance_},
      cfg_{cfg_},
      sck_pin_{sck_p},
      miso_pin_{miso_p},
      mosi_pin_{mosi_p}
{
}

LBR::Stml4::HwSpi LBR::Stml4::SpiModule::CreateSpi()
{
    // Instantiate new SPI object

    // Use the class members (they have trailing underscores)
    LBR::Stml4::HwSpi spi{instance_, cfg_};

    // Init SPI gpio pins
    sck_pin_.init();
    miso_pin_.init();
    mosi_pin_.init();

    // Init SPI peripheral before returning the object
    spi.Init();

    return spi;
}
}  // namespace LBR::Stml4