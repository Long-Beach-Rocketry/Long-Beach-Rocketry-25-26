/**
 * @file st_spi_factory.h
 * @author Kent Hong
 * @brief SPI Module that creates valid SPI objects for use in the BSP
 * @date 2025-10-01
 *
 */
#pragma once

#include "st_gpio.h"
#include "st_spi.h"

namespace LBR
{
namespace Stml4
{
class SpiModule
{
public:
    explicit SpiModule(SPI_TypeDef* instance_, StSpiSettings& cfg_,
                       StGpioParams& sck_p, StGpioParams& miso_p,
                       StGpioParams& mosi_p);
    HwSpi CreateSpi();
    HwSpi GetSpi();

private:
    SPI_TypeDef* instance;
    StSpiSettings cfg;
    HwGpio sck_pin;
    HwGpio miso_pin;
    HwGpio mosi_pin;
};
}  // namespace Stml4
}  // namespace LBR