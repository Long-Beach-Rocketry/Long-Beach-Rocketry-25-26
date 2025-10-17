/**
 * @file    st_spi.h
 * @author  Bex Saw
 * @brief   Bare-metal STM32L4 SPI driver (implements Spi interface)
 * @version 0.2
 * @date    2025-10-05
 *
 * @note Usage:
 *   LBR::Stml4::HwSpi spi(SPI1, settings);
 *   spi.Init();
 *   spi.Send(tx_buf, len, 1000);
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include "spi.h"
#include "stm32l476xx.h"

namespace LBR
{
namespace Stml4
{

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

enum class SpiBusMode : uint8_t
{
    MODE1 = 0,  // CPOL=0, CPHA=0
    MODE2,      // CPOL=0, CPHA=1
    MODE3,      // CPOL=1, CPHA=0
    MODE4       // CPOL=1, CPHA=1
};

enum class SpiBitOrder : uint8_t
{
    MSB = 0,
    LSB
};

enum class SpiRxThreshold : uint8_t
{
    FIFO_16bit = 0,
    FIFO_8bit
};

enum class SpiStatus : uint8_t
{
    OK = 0,
    READ_ERR,
    WRITE_ERR,
    TRANSFER_ERR,
    INIT_ERR
};

struct StSpiSettings
{
    SpiBaudRate baudrate;
    SpiBusMode busmode;
    SpiBitOrder order;
    SpiRxThreshold threshold;
};

class HwSpi : public Spi
{
    friend bool ValidateSpi(HwSpi& spi);

public:
    explicit HwSpi(SPI_TypeDef* instance, const StSpiSettings& cfg);

    bool Init();
    bool Read(uint8_t* rx_buf, size_t len);
    bool Write(const uint8_t* tx_buf, size_t len);
    bool Transfer(const uint8_t* tx_buf, uint8_t* rx_buf, size_t len);

    /* Parameterless overrides to satisfy LBR::Spi interface (legacy API).
     * These are thin wrappers that perform a zero-length operation by
     * delegating to the parameterized methods. They avoid changing the
     * shared `spi.h` interface while allowing HwSpi to be instantiated.
     */
    bool Read() override;
    bool Write() override;
    bool Transfer() override;

private:
    SPI_TypeDef* instance;
    StSpiSettings settings;

    static inline void SetReg(volatile uint32_t* reg, uint32_t val,
                              uint32_t pos, uint32_t width);
};
}  // namespace Stml4
}  // namespace LBR
