/**
 * @file st_spi_module.h
 * @author Bex Saw
 * @brief Factory module for creating and configuring SPI interfaces and their GPIO pins.
 * @date  2025-10-01
 */

#pragma once

#include "st_gpio.h"
#include "st_spi.h"

namespace LBR::Stml4
{

/**
 * @class SpiModule
 * @brief Handles SPI initialization and GPIO configuration for SCK, MISO, and MOSI pins.
 */
class SpiModule
{
public:
    /**
     * @brief Construct a new SPI module.
     * @param instance_ Pointer to the SPI peripheral instance.
     * @param cfg_ SPI configuration settings.
     * @param sck_p GPIO parameters for the SCK pin.
     * @param miso_p GPIO parameters for the MISO pin.
     * @param mosi_p GPIO parameters for the MOSI pin.
     */
    explicit SpiModule(SPI_TypeDef* instance_, const StSpiSettings& cfg_,
                       const StGpioParams& sck_p, const StGpioParams& miso_p,
                       const StGpioParams& mosi_p);

    /**
     * @brief Create a new SPI hardware object based on the configuration.
     * @return HwSpi The initialized SPI object.
     */
    HwSpi CreateSpi();

    /**
     * @brief Retrieve the SPI object if already created and valid.
     * @return HwSpi The existing SPI instance.
     */
    HwSpi GetSpi() const;

private:
    SPI_TypeDef* instance_;
    StSpiSettings cfg_;
    HwGpio sck_pin_;
    HwGpio miso_pin_;
    HwGpio mosi_pin_;
};

}  // namespace LBR::Stml4
