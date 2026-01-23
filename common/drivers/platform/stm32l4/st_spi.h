/**
 * @file st_spi.h
 * @author Kent Hong
 * @brief SPI Driver library for STM32l4xx
 * @date 2025-09-28
 *
 */

#pragma once

#include <stdbool.h>
#include <cstddef>
#include <span>
#include "reg_helpers.h"
#include "spi.h"
#include "stm32l476xx.h"

namespace LBR
{
namespace Stml4
{

/**
 * @brief SPI Clock Baud Rate setting where FPCLK_n is the peripheral clock
 * frequency / n
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
 * @brief SPI bus mode setting
 *        MODE1 = CPOL - 0 and CPHA - 0
 *        MODE2 = CPOL - 0 and CPHA - 1
 *        MODE3 = CPOL - 1 and CPHA - 0
 *        MODE4 = CPOL - 1 and CPHA - 1
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
 * @brief Receive Most Significant Bit or Least Significant Bit through data
 * transfer
 *
 */
enum class SpiBitOrder : uint8_t
{
    MSB = 0,
    LSB
};

/**
 * @brief FIFO Rx Threshold determines how many bits in the RX buffer triggers
 * an RXNE event (new data is ready to be read)
 *
 */
enum class SpiRxThreshold : uint8_t
{
    FIFO_16bit = 0,
    FIFO_8bit
};

/**
 * @brief SPI status codes for error checking
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

/**
 * @brief Desired SPI control register settings
 *
 */
struct StSpiSettings
{
    SpiBaudRate baudrate;
    SpiBusMode busmode;
    SpiBitOrder order;
    SpiRxThreshold threshold;
};

class HwSpi : public Spi
{
public:
    /**
    * @brief Construct a new HwSpi object
    *
    * @param instance_ The SPI peripheral being used
    * @param settings_ The SPI control register settings
    *
    */
    explicit HwSpi(SPI_TypeDef* instance_, StSpiSettings& settings_);

    /**
    * @brief read data from a slave device.
    * 
    * @param rx_data 8 byte array to store read data.
    * @param buffer_len Size of array.
    * @return true SPI read success, false read failed
    */
    bool read(std::span<uint8_t> rx_data) override;

    /**
    * @brief write data to a slave device.
    * 
    * @param tx_data 8 byte array of the data to be sent.
    * @param buffer_len Size of array
    * @return true SPI write success, false write failed
    */
    bool write(std::span<uint8_t> tx_data) override;

    /**
    * @brief Sequential write and read data to a slave device.
    * 
    * @param tx_data 8 byte array of the data to be sent.
    * @param rx_data 8 byte array to store read data.
    * @param buffer_len Size of arrays
    * @return true SPI Sequential transfer success, false Sequential transfer failed
    */
    bool seq_transfer(std::span<uint8_t> tx_data,
                      std::span<uint8_t> rx_data) override;

    /**
    * @brief Initializes SPI peripheral and its sck, mosi, miso, and nss pins
    *
    * @return true Initialization success, false Initialization failed
    */
    bool init();

private:
    // Member variables
    SPI_TypeDef* instance;
    StSpiSettings settings;
};
}  // namespace Stml4
}  // namespace LBR