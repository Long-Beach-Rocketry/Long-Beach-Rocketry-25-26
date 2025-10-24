/**
 * @file st_spi.h
 * @author Kent Hong
 * @brief SPI Driver library for STM32l4xx
 * @date 2025-09-28
 *
 */

#pragma once

#include <stdbool.h>
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
    READ_ERR,
    WRITE_ERR,
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
    explicit HwSpi(SPI_TypeDef* instance_, StSpiSettings& settings_);


    // Member Functions
    bool Init();
    bool Read(uint8_t* rx_data, size_t buffer_len) override;
    bool Write(uint8_t* tx_data, size_t buffer_len) override;
    bool Transfer(uint8_t* tx_data, uint8_t* rx_data,
                  size_t buffer_len) override;

private:
    // Member variables
    SPI_TypeDef* instance;
    StSpiSettings settings;

    // Private Member Functions
    static inline void SetReg(volatile uint32_t* reg, uint32_t enum_val,
                              uint32_t bit_num, uint32_t bit_length);
};
}  // namespace Stml4
}  // namespace LBR