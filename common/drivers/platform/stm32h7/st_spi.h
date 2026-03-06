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

//Spi settings enums and struct
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

//SPI bus modes (clock polarity and phase)
enum class SpiBusMode : uint8_t
{
    MODE1 = 0,
    MODE2,
    MODE3,
    MODE4
};

//SPI bit order
enum class SpiBitOrder : uint8_t
{
    MSB = 0,
    LSB
};

//SPI RX threshold for FIFO
enum class SpiRxThreshold : uint8_t
{
    FIFO_16bit = 0,
    FIFO_8bit
};

//SPI settings struct
struct StSpiSettings
{
    SpiBaudRate baudrate;
    SpiBusMode busmode;
    SpiBitOrder order;
    SpiRxThreshold threshold;
};

//SPI driver class for STM32H723 board
class HwSpi : public Spi
{
public:
    //constructor takes SPI instance and settings struct
    explicit HwSpi(SPI_TypeDef* instance_, const StSpiSettings& settings_);

    bool read(std::span<uint8_t> rx_data) override;
    bool write(std::span<uint8_t> tx_data) override;
    bool seq_transfer(std::span<uint8_t> tx_data,
                      std::span<uint8_t> rx_data) override;

    bool init();

private:
    SPI_TypeDef* instance;
    StSpiSettings settings;
};

}  // namespace Stmh7
}  // namespace LBR
