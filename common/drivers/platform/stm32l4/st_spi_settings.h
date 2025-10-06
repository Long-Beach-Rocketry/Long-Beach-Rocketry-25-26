/**
 * @file st_spi_settings.h
 * @author Bex Saw
 * @brief STM32L4 bare-metal SPI configuration definitions
 * @version 0.1
 * @date 2025-10-01
 */

#pragma once

#include "stm32l4xx.h"
#include <cstdint>

/**
 * @brief SPI baud rate prescaler values (divides FPCLK).
 */
enum class SpiBaudRate : uint8_t {
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
 * @brief SPI clock polarity and phase combinations.
 * 
 * - MODE1: CPOL=0, CPHA=0
 * - MODE2: CPOL=0, CPHA=1
 * - MODE3: CPOL=1, CPHA=0
 * - MODE4: CPOL=1, CPHA=1
 */
enum class SpiBusMode : uint8_t {
    MODE1 = 0,
    MODE2,
    MODE3,
    MODE4
};

/**
 * @brief SPI bit transmission order.
 */
enum class SpiBitOrder : uint8_t {
    MSB_FIRST = 0,
    LSB_FIRST
};

/**
 * @brief RX FIFO threshold configuration.
 */
enum class SpiRxThreshold : uint8_t {
    RX_8BIT = 0,
    RX_16BIT
};

/**
 * @brief SPI driver status codes.
 */
enum class SpiStatus : uint8_t {
    OK = 0,
    READ_ERR,
    WRITE_ERR,
    TRANSFER_ERR,
    INIT_ERR
};

/**
 * @brief Data frame size configuration.
 */
enum class SpiDataSize : uint8_t {
    DATA_8BIT = 0,
    DATA_16BIT
};

/**
 * @brief User-defined SPI configuration structure.
 */
struct StSpiSettings {
    SpiBaudRate baudRate   = SpiBaudRate::FPCLK_16;
    SpiBusMode busMode     = SpiBusMode::MODE1;
    SpiDataSize dataSize   = SpiDataSize::DATA_8BIT;
    SpiBitOrder bitOrder   = SpiBitOrder::MSB_FIRST;
    SpiRxThreshold rxThreshold = SpiRxThreshold::RX_8BIT;
};
