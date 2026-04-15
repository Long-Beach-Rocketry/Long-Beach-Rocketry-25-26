/**
 * @file st_spi.h
 * @brief SPI Driver library for STM32H723
 * @author Nick Terrazas
 */

#pragma once

#include <cstddef>
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
 * @brief These correspond to the division factor of the peripheral clock used to generate the SPI clock.
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
 * @brief These correspond to the clock polarity and phase settings for SPI communication.   
 */
enum class SpiBusMode : uint8_t
{
    MODE1 = 0,
    MODE2,
    MODE3,
    MODE4
};

/**
 * @brief This determines whether the most significant bit or least significant bit is transmitted first.
 */
enum class SpiBitOrder : uint8_t
{
    MSB = 0,
    LSB
};

/**
 * @brief This determines the receive FIFO threshold for SPI communication.
 */
enum class SpiRxThreshold : uint8_t
{
    FIFO_16bit = 0,
    FIFO_8bit
};

/**
 * @brief Struct to hold SPI settings for initialization
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
     * @brief Initializes the SPI peripheral with the specified settings.
     * 
     * @param instance_ pointer to the SPI instance 
     * @param settings_ struct containing the SPI settings for initialization
     */
    explicit HwSpi(SPI_TypeDef* instance_, const StSpiSettings& settings_);

    /**
     * @brief Implements the read function from the Spi interface. 
     * Uses simple polling with timeout to read data from the SPI peripheral.
     * @param rx_data span to store the received data
     * @return True on successful read, false otherwise.
     */
    bool read(std::span<uint8_t> rx_data) override;

    /**
     * @brief Implements the write function from the Spi interface.
     * @param tx_data An uint8_t std::span data to be sent.
     * @return True on success. False, otherwise.
     */
    bool write(std::span<uint8_t> tx_data) override;

    /**
     * @brief Implements the seq_transfer function from the Spi interface. 
     * @param tx_data An uint8_t std::span data to be sent.
     * @param rx_data span to store the received data
     * @return True on success. False, otherwise.
     */
    bool seq_transfer(std::span<uint8_t> tx_data,
                      std::span<uint8_t> rx_data) override;

    bool init();

private:
    SPI_TypeDef* instance; //Pointer to the SPI peripheral's base address
    StSpiSettings settings; //Struct containing the SPI settings for initialization
};

}  // namespace Stmh7
}  // namespace LBR
