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
class SpiModule
{
public:
    explicit SpiModule(SPI_TypeDef* instance_, StSpiSettings& cfg_,
                       HwGpio& sck_pin_, HwGpio& cs_pin_, HwGpio& miso_pin_,
                       HwGpio& mosi_pin_);
    HwSpi CreateSpi();
    HwSpi GetSpi();

private:
    SPI_TypeDef* instance;
    StSpiSettings cfg;
    HwGpio sck_pin;
    HwGpio cs_pin;
    HwGpio miso_pin;
    HwGpio mosi_pin;
};
}  // namespace Stml4
}  // namespace LBR