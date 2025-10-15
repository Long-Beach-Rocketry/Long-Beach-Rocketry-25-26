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

HwSpi SpiModule::CreateSpi()
{
    return HwSpi{instance_, cfg_};
}

HwSpi SpiModule::GetSpi() const
{
    HwSpi spi = HwSpi{instance_, cfg_};

    if (instance_ && (instance_->CR1 & SPI_CR1_SPE))
    {
        // Already enabled or valid SPI
        return spi;
    }

    // You can call Init() here if you want to auto-initialize
    return spi;
}

bool LBR::Stml4::SpiModule::ValidateSpi(const HwSpi& spi)
{
    if (!instance_)
        return false;

    // Ensure peripheral clock is enabled (STM32L4 register map)
    const bool clk_enabled =
        (instance_ == SPI1 && (RCC->APB2ENR & RCC_APB2ENR_SPI1EN)) ||
        (instance_ == SPI2 && (RCC->APB1ENR1 & RCC_APB1ENR1_SPI2EN)) ||
        (instance_ == SPI3 && (RCC->APB1ENR1 & RCC_APB1ENR1_SPI3EN));

    if (!clk_enabled)
        return false;

    if (&sck_pin_ == nullptr || &miso_pin_ == nullptr || &mosi_pin_ == nullptr)
        return false;

    (void)cfg_;
    return true;
}
}  // namespace LBR::Stml4