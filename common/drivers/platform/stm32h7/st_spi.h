/**
 * @file st_spi.h
 * @brief SPI Driver library for STM32H723xx
 *
 */

#pragma once

#include <cstdint>
#include <span>
#include "reg_helpers.h"
#include "spi.h"
#include "stm32h723xx.h"

namespace LBR
{
namespace Stmh7
{

/**
 * @brief SPI clock baud rate setting, where FPCLK_n is the peripheral clock
 * frequency divided by n. Maps directly to MBR[2:0] in SPI_CFG1.
 *
 */
enum class SpiBaudRate : uint8_t
{
    FPCLK_2 = 0,
    FPCLK_4,
    FPCLK_8,
    FPCLK_16,
    FPCLK_32,
    FPCLK_64,
    FPCLK_128,
    FPCLK_256
};

/**
 * @brief SPI bus mode setting, determined by clock polarity (CPOL) and clock
 * phase (CPHA). Maps to CPOL and CPHA bits in SPI_CFG2.
 *        MODE1: CPOL = 0, CPHA = 0 
 *        MODE2: CPOL = 0, CPHA = 1 
 *        MODE3: CPOL = 1, CPHA = 0 
 *        MODE4: CPOL = 1, CPHA = 1 
 *
 */
enum class SpiBusMode : uint8_t
{
    MODE1 = 0,
    MODE2,
    MODE3,
    MODE4
};

/**
 * @brief Bit order for SPI data transfer. Controls whether the Most
 * Significant Bit or Least Significant Bit is transmitted first.
 * Maps to LSBFRST bit in SPI_CFG2.
 *
 */
enum class SpiBitOrder : uint8_t
{
    MSB = 0,
    LSB
};

/**
 * @brief Number of bits per SPI data frame. Values are the raw register
 * encoding for DSIZE[4:0] in SPI_CFG1, where an N-bit frame is encoded
 * as N-1. Valid frame sizes are 4 to 32 bits for SPI1/2/3, and 4 to 16
 * bits for SPI4/5/6.
 *
 */
enum class SpiDataSize : uint8_t
{
    DSIZE4 = 3,
    DSIZE8 = 7,
    DSIZE16 = 15,
    DSIZE32 = 31
};

/**
 * @brief FIFO threshold level defining how many data frames constitute one
 * packet event, triggering RXP/TXP flags. Maps to FTHLV[3:0] in SPI_CFG1.
 * Values are raw register encodings where FIFO_N = N-1.
 * SPI1/2/3 support up to 16 frames; SPI4/5/6 support up to 8 frames.
 *
 */
enum class SpiRxThreshold : uint8_t
{
    FIFO_1  = 0,  
    FIFO_2 = 1,
    FIFO_4 = 3,
    FIFO_8 = 7,
    FIFO_16 = 15        
};


/**
 * @brief Desired SPI control register settings to be applied during init().
 *
 */
struct StSpiSettings
{
    SpiBaudRate baudrate;   // CFG1: MBR[2:0]
    SpiBusMode busmode;    // CFG2: CPOL + CPHA
    SpiBitOrder order;      // CFG2: LSBFRST
    SpiDataSize datasize;   // CFG1: DSIZE[4:0]
    SpiRxThreshold threshold; // CFG1: FTHLV[3:0]
};

/**
 * @brief SPI status codes returned to indicate the result of an operation.
 *
 */
enum class SpiStatus : uint8_t
{
    OK = 0,
    read_ERR,
    write_ERR,
    TRANSFER_ERR,
    INIT_ERR
};

class HwSpi : public Spi
{
public:
    /**
     * @brief Construct a new HwSpi object.
     *
     * @param instance_ Pointer to the SPI peripheral to use (e.g. SPI1, SPI2)
     * @param settings_ Struct containing the desired SPI configuration
     */
    explicit HwSpi(SPI_TypeDef* instance_, StSpiSettings& settings_);

    /**
     * @brief Read data from a slave device.
     *
     * @param rx_data Buffer to store received bytes
     * @return true on success, false on read failure
     */
    bool read(std::span<uint8_t> rx_data) override;

    /**
     * @brief Write data to a slave device.
     *
     * @param tx_data Buffer containing bytes to transmit
     * @return true on success, false on write failure
     */
    bool write(std::span<uint8_t> tx_data) override;

    /**
     * @brief Perform a simultaneous write and read (full-duplex transfer).
     *
     * @param tx_data Buffer containing bytes to transmit
     * @param rx_data Buffer to store received bytes
     * @return true on success, false on transfer failure
     */
    bool seq_transfer(std::span<uint8_t> tx_data,
                      std::span<uint8_t> rx_data) override;

    /**
     * @brief Initialize the SPI peripheral and configure its GPIO pins
     * (SCK, MOSI, MISO, NSS) according to the settings provided at
     * construction. Must be called before any read/write operations.
     *
     * @return true on success, false on initialization failure
     */
    bool init();

private:
    SPI_TypeDef*  instance; // Pointer to the SPI peripheral registers
    StSpiSettings settings; // Copy of configuration settings
};


}   // namespace Stmh7
}   // namespace LBR