/**
 * @file st_spi_defs.h
 * @author Kent Hong
 * @brief SPI Driver defines that contain CR1/CR2 register settings and a struct to store the desired settings
 * @version 0.1
 * @date 2025-09-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include "stm32l4xx.h"
#include <cstdint>

/**
 * @brief SPI Clock Baud Rate setting where FPCLK_n is the peripheral clock frequency / n
 * 
 */
enum class SpiBaudRate : std::uint8_t {
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
enum class SpiBusMode : std::uint8_t {
    MODE1 = 0,
    MODE2,
    MODE3,
    MODE4
};

/**
 * @brief SPI read, write, and transfer data size settings
 * 
 */
enum class SpiDataSize : std::uint8_t {
    ONE_BIT = 0,
    TWO_BIT,
    THREE_BIT,
    FOUR_BIT,
    FIVE_BIT,
    SIX_BIT,
    SEVEN_BIT,
    EIGHT_BIT,
    NINE_BIT,
    TEN_BIT,
    ELEVEN_BIT,
    TWELVE_BIT,
    THIRTEEN_BIT,
    FOURTEEN_BIT,
    FIFTEEN_BIT,
    SIXTEEN_BIT
};

/**
 * @brief Receive Most Significant Bit or Least Significant Bit through data transfer
 * 
 */
enum class SpiBitOrder : std::uint8_t {
    MSB = 0,
    LSB
};

/**
 * @brief FIFO Rx Threshold determines how many bits in the RX buffer triggers an RXNE event (new data is ready to be read)
 * 
 */
enum class SpiRxThreshold : std::uint8_t {
    FIFO_16bit = 0,
    FIFO_8bit       
};

/**
 * @brief SPI status codes for error checking
 * 
 */
enum class SpiStatus : std::uint8_t {
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
struct SpiCrSettings {
    SpiBaudRate baudrate;
    SpiBusMode busmode;
    SpiDataSize datasize;
    SpiBitOrder order;
    SpiRxThreshold threshold;
};