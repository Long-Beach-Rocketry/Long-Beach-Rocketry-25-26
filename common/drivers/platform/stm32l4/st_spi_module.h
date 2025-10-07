/**
 * @file st_spi_factory.h
 * @author Kent Hong
 * @brief SPI Factory that creates valid SPI objects for use in the BSP
 * @version 0.1
 * @date 2025-10-01
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "st_gpio.h"
#include "st_spi.h"

namespace LBR
{
namespace Stml4
{
HwSpi CreateSpi(SPI_TypeDef* instance, SpiCrSettings& cfg);
HwSpi GetSpi(SPI_TypeDef* instance, SpiCrSettings& cfg);
}  // namespace Stml4
}  // namespace LBR