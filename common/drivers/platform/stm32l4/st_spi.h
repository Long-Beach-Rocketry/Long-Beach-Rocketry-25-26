/**
 * @file st_spi.h
 * @author Kent Hong
 * @brief SPI Driver library for STM32l4xx
 * @version 0.1
 * @date 2025-09-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "spi.h"
#include "st_spi_settings.h"
#include <stdbool.h>
#include <stdint.h>

namespace LBR {
namespace Stml4 {
class HwSpi : public Spi {
  friend bool ValidateSpi(HwSpi &spi);
  friend void SpiConfigSettings(HwSpi &spi, const SpiCrSettings &cfg);

public:
  explicit HwSpi();
  explicit HwSpi(SPI_TypeDef *instance_, SpiCrSettings &settings_);

  // Member Functions
  SpiStatus Init();
  // TODO: implement read, write, and transfer for SPI
  bool Read();
  bool Write(std::uint16_t output_data);
  bool Transfer(std::uint16_t output_data);

  // Setter and Getter
  bool IsSpiValid() const;
  void SetSpiValid(bool initialized_);

private:
  // Member variables
  SPI_TypeDef *instance;
  SpiCrSettings settings;
  bool initialized;

  // Private Member Functions
  // TODO: Replace the switch statements in these functions
  bool SetSpiBaudRate(SpiBaudRate baudrate);
  bool SetSpiBusMode(SpiBusMode mode);
};
} // namespace Stml4
} // namespace LBR