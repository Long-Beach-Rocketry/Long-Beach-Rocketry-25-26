/**
 * @file st_spi.cc
 * @author Kent Hong
 * @brief SPI Driver class implementation
 * @version 0.1
 * @date 2025-09-30
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "st_spi.h"
namespace LBR {
namespace Stml4 {
/**
 * @brief Default constructor that signals HwSpi object failed to initialize
 *
 */
explicit HwSpi() : instance(nullptr), initialized(false) {}

/**
 * @brief Construct a new HwSpi object
 *
 * @param instance_ The SPI peripheral being used
 * @param settings_ The SPI control register settings
 *
 */
HwSpi::HwSpi(SPI_TypeDef *instance_, SpiCrSettings &settings_)
    : instance(instance_), settings(settings_), initialized(true) {}

/**
 * @brief Getter
 *
 * @return SpiPins const
 */
SpiPins HwSpi::GetPins() const { return spi_pins; }

/**
 * @brief Get the value that shows whether this object was initialized
 * successfully or not
 *
 * @return true
 * @return false
 */
bool HwSpi::GetSpiValid() const { return initialized; }

/**
 * @brief Setter
 *
 * @param pins
 */
void HwSpi::SetPins(const SpiPins &spi_pins_) { spi_pins = spi_pins_; }

/**
 * @brief Sets the SPI serial clock baudrate for bits 5:3 in SPIx_CR1
 *
 * @param baudrate
 */
bool HwSpi::SetSpiBaudRate(SpiBaudRate baudrate) {

  // Clear bits 5:3 to 0b000
  instance->CR1 &= ~(SPI_CR1_BR);

  switch (baudrate) {
  case SpiBaudRate::FPCLK_2:
    break; // 0b000 << 3
  case SpiBaudRate::FPCLK_4:
    instance->CR1 |= SPI_CR1_BR_0; // 0b001 << 3
    break;
  case SpiBaudRate::FPCLK_8:
    instance->CR1 |= SPI_CR1_BR_1; // 0b010 << 3
    break;
  case SpiBaudRate::FPCLK_16:
    instance->CR1 |= (SPI_CR1_BR_1 | SPI_CR1_BR_0); // 0b011 << 3
    break;
  case SpiBaudRate::FPCLK_32:
    instance->CR1 |= SPI_CR1_BR_2; // 0b100 << 3
    break;
  case SpiBaudRate::FPCLK_64:
    instance->CR1 |= (SPI_CR1_BR_2 | SPI_CR1_BR_0); // 0b101 << 3
    break;
  case SpiBaudRate::FPCLK_128:
    instance->CR1 |= (SPI_CR1_BR_2 | SPI_CR1_BR_1); // 0b110 << 3
    break;
  case SpiBaudRate::FPCLK_256:
    instance->CR1 |= SPI_CR1_BR; // 0b111 << 3
    break;
  default:
    return false;
  }
  return true;
}

/**
 * @brief Sets the SPI Bus Mode for Bit 1 - CPOL and Bit 0 - CPHA
 *
 * @param mode The bus mode from the SpiBusMode enum class
 * @return true Bus Mode Configuration success
 * @return false Bus Mode Configuration failed
 *
 * @note The bus mode is dependent on the slave device you are interfacing with
 * (in other words, check its datasheet)
 */
bool HwSpi::SetSpiBusMode(SpiBusMode mode) {

  switch (mode) {
  case SpiBusMode::MODE1:
    instance->CR1 &= ~(SPI_CR1_CPHA); // CPHA = 0
    instance->CR1 &= ~(SPI_CR1_CPOL); // CPOL = 0
    break;
  case SpiBusMode::MODE2:
    instance->CR1 |= SPI_CR1_CPHA;    // CPHA = 1
    instance->CR1 &= ~(SPI_CR1_CPOL); // CPOL = 0
    break;
  case SpiBusMode::MODE3:
    instance->CR1 &= ~(SPI_CR1_CPHA); // CPHA = 0
    instance->CR1 |= SPI_CR1_CPOL;    // CPOL = 1
    break;
  case SpiBusMode::MODE4:
    instance->CR1 |= SPI_CR1_CPHA; // CPHA = 1
    instance->CR1 |= SPI_CR1_CPOL; // CPOL = 1
  default:
    return false;
  }
  return true;
}

/**
 * @brief Configure SPI settings to implement into CR1 and CR2
 *
 * @param cfg SpiCrSettings struct with desired config values for the registers
 * in CR1 and CR2
 */
void HwSpi::SpiConfigSettings(const SpiCrSettings &cfg) { settings = cfg; }

/**
 * @brief Initializes SPI peripheral and its sck, mosi, miso, and nss pins
 *
 * @return SpiStatus::Ok initialization success
 * @return SpiStatus::InitErr initialization failed
 */
SpiStatus HwSpi::Init() {

  // Set to Master Mode (Keep in master mode unless we want our STM32l4xx to be
  // used as a slave device) Master Mode - Sets our STM32l4xx to act as the
  // master device to initiate/end SPI communication and drive the clock signal
  instance->CR1 |= SPI_CR1_MSTR;

  // Enable Full-Duplex Mode (Can send and receive data simultaneously through
  // MOSI and MISO)
  instance->CR1 &= ~(SPI_CR1_RXONLY);

  // Configure SPI Baudrate
  if (!SetSpiBaudRate(settings.baudrate)) {
    return SpiStatus::INIT_ERR;
  }

  // Configure SPI Bus Mode
  if (!SetSpiBusMode(settings.busmode)) {
    return SpiStatus::INIT_ERR;
  }

  // Configure SPI data size to 8 bits
  instance->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2);

  // Use software slave management and toggle SSI = 1 to pull NSS to high
  // Toggle SSI = 0 when we want to select the slave device for data transfer
  instance->CR1 |= SPI_CR1_SSM;
  instance->CR1 |= SPI_CR1_SSI;

  // Configure Bit order
  if (settings.order == SpiBitOrder::MSB) {
    instance->CR1 &= ~(SPI_CR1_LSBFIRST);
  } else if (settings.order == SpiBitOrder::LSB) {
    instance->CR1 |= SPI_CR1_LSBFIRST;
  }

  // Determine FIFO reception threshold to see how many bits in RX Buffer
  // triggers an RXNE event
  if (settings.threshold == SpiRxThreshold::FIFO_16bit) {
    instance->CR2 &= ~(SPI_CR2_FRXTH);
  } else if (settings.threshold == SpiRxThreshold::FIFO_8bit) {
    instance->CR2 |= SPI_CR2_FRXTH;
  }

  // Enable SPI peripheral
  instance->CR1 |= SPI_CR1_SPE;

  return SpiStatus::OK;
}
} // namespace Stml4
} // namespace LBR