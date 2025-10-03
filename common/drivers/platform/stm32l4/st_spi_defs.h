/**
 * @file st_spi_defs.h
 * @author Bex Saw
 * @brief SPI definitions that are specific to the STM32L4 series
 * @version 0.1
 * @date 2025-10-01
 * 
 */

 #pragma once

 #include "stm32l4xx.h"
 #include <cstdint>

 /**
  * @brief These settings control how fast the SPI clock runs. The speed is set by dividing the main clock (FPCLK) by a number (n).
  * 
  */
// Baud Rate Control
enum class SpiBaudRate : std::uint8_t 
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

// CPOL & CPHA
/**
 * @brief SPI bus mode settings for Clock Polarity (CPOL) and Clock Phase (CPHA).
 */

enum class SpiBusMode : std::uint8_t {

    MODE1 = 0, // CPOL=0, CPHA=0
    MODE2,     // CPOL=0, CPHA=1
    MODE3,     // CPOL=1, CPHA=0
    MODE4      // CPOL=1, CPHA=1
};

/** 
 * @brief Recieve MSB/LSB first through data transfer
 * Purpose: This setting determines the order in which bits are transmitted and received over the SPI bus.
 */

 enum class SpiBitOrder : std::uint8_t {
    MSB_FIRST = 0,
    LSB_FIRST
};


/**
 * @brief If the number of bits in the RX buffer reaches this threshold, it means new data is ready to be read.
 * FIFO (RX Buffer): can hold multiple bytes, so this threshold helps manage when to read data.
 */

 enum class SpiRxThreshold : std::uint8_t {
    RX_8BIT = 0,   // 8-bit threshold
    RX_16BIT       // 16-bit threshold
};

/**
 * @brief SPI device error checking and handling
 * Purpose: These settings help manage and respond to errors that may occur during SPI communication.
 */
enum class SpiStatus : std::uint8_t {
    OK = 0,           // No error
    OVERRUN_ERR,      // Data overrun error
    MODE_FAULT_ERR,   // Mode fault error
    CRC_ERR,          // CRC error
    UNEXPECTED_ERR    // Any other unexpected error
};

/**
 * @brief SPI data frame size configuration
 * Purpose: This setting determines how many bits are in each data frame transmitted or received over the SPI bus.
 */
enum class SpiDataSize : std::uint8_t {
        DATA_8BIT = 0,   // 8-bit data frame
        DATA_16BIT       // 16-bit data frame
};  

/**
 * @brief SPI control register settings and configurations.
 * Purpose: These settings configure how the SPI peripheral operates.
 */
 struct StSpiSettings {
    SpiBaudRate baudRate;      // Baud rate control
    SpiBusMode busMode;       // Bus mode (CPOL/CPHA)
    SpiDataSize dataSize;     // Data frame size (8-bit or 16-bit)
    SpiBitOrder bitOrder;     // Bit order (MSB/LSB first)
    SpiRxThreshold rxThreshold; // RX buffer threshold
 };

