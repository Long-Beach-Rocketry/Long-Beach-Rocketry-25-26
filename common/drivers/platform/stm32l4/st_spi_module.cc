/**
 * @file st_spi_module.cc
 * @author Bex Saw
 * @brief SPI Module implementation for STM32L4 bare-metal environment.
 * @date 2025-10-07
 */

#include "st_spi_module.h"
#include "stm32l4xx.h"

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
 * @brief Create a new SPI hardware object with provided configuration.
 */

LBR::Stml4::HwSpi LBR::Stml4::SpiModule::CreateSpi()
{
    return LBR::Stml4::HwSpi{instance, cfg};
}

/**
 * @brief Return an initialized SPI object if validation passes.
 */

LBR::Stml4::HwSpi LBR::Stml4::SpiModule::GetSpi()
{
    LBR::Stml4::HwSpi spi = CreateSpi();

    if (ValidateSpi(spi))
    {
        spi.Init();
    }
    else
    {
        // TODO: Handle invalid SPI
    }

    return spi;
}

/**
 * @brief Validate SPI hardware and GPIO state before initialization.
 * @param spi Reference SPI object.
 * @return true if peripheral is valid and safe to initialize.
 */
bool LBR::Stml4::SpiModule::ValidateSpi(const HwSpi& spi)
{
    // Null or unassigned peripheral
    if (!instance)
        return false;

    // Ensure peripheral clock is enabled (STM32L4 register map)
    bool clk_enabled =
        (instance == SPI1 && (RCC->APB2ENR & RCC_APB2ENR_SPI1EN)) ||
        (instance == SPI2 && (RCC->APB1ENR1 & RCC_APB1ENR1_SPI2EN)) ||
        (instance == SPI3 && (RCC->APB1ENR1 & RCC_APB1ENR1_SPI3EN));

    if (!clk_enabled)
        return false;

    // GPIO pin sanity
    if (!sck_pin.IsValid() || !miso_pin.IsValid() || !mosi_pin.IsValid())
        return false;

    // Avoid re-init if already enabled
    if (instance->CR1 & SPI_CR1_SPE)
        return false;

    // Basic configuration sanity
    if (!cfg.baudRatePrescaler || !cfg.direction)
        return false;

    return true;
}
