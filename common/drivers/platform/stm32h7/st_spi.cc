/**
 * @file st_spi.cc
 * @brief SPI Driver class implementation for STM32H723
 * @author Nick Terrazas
 */

#include "st_spi.h"

namespace
{
//Timeout for waiting on SPI flags
constexpr uint32_t kSpiTimeoutCycles = 1000000u;

//wait_for_flag helper function to poll a register flag with timeout
bool wait_for_flag(volatile uint32_t* reg, uint32_t flag)
{
    uint32_t timeout = kSpiTimeoutCycles;
    while (((*reg) & flag) == 0u)
    {
        if (timeout-- == 0u)
        {
            return false;
        }
    }
    return true;
}

//Helper functions to read/write 8-bit data from/to SPI data registers
inline void write_txdr8(SPI_TypeDef* spi, uint8_t value)
{
    *(volatile uint8_t*)&spi->TXDR = value;
}

inline uint8_t read_rxdr8(SPI_TypeDef* spi)
{
    return *(volatile uint8_t*)&spi->RXDR;
}

//Helper functions to start and finish SPI transfers
bool start_transfer(SPI_TypeDef* spi, uint16_t frame_count)
{
    SetReg(&spi->CR2, frame_count, SPI_CR2_TSIZE_Pos, 16);
    spi->IFCR = SPI_IFCR_EOTC | SPI_IFCR_TXTFC | SPI_IFCR_OVRC;
    spi->CR1 |= SPI_CR1_CSTART;
    return true;
}

bool finish_transfer(SPI_TypeDef* spi)
{
    if (!wait_for_flag(&spi->SR, SPI_SR_EOT))
    {
        return false;
    }

    spi->IFCR = SPI_IFCR_EOTC | SPI_IFCR_TXTFC | SPI_IFCR_OVRC;
    return true;
}
}  // namespace

namespace LBR
{
namespace Stmh7
{

HwSpi::HwSpi(SPI_TypeDef* instance_, const StSpiSettings& settings_)
    : instance{instance_}, settings{settings_}
{
}

//read, write, and seq_transfer implementations use simple polling with timeout
bool HwSpi::read(std::span<uint8_t> rx_data)
{
    if (instance == nullptr)
    {
        return false;
    }

    if ((instance->CR1 & SPI_CR1_SPE) == 0u)
    {
        return false;
    }

    if (rx_data.empty())
    {
        return true;
    }

    if (rx_data.size() > 0xFFFFu)
    {
        return false;
    }

    if (!start_transfer(instance, static_cast<uint16_t>(rx_data.size())))
    {
        return false;
    }

    for (auto& byte : rx_data)
    {
        if (!wait_for_flag(&instance->SR, SPI_SR_TXP))
        {
            return false;
        }
        write_txdr8(instance, 0x00u);

        if (!wait_for_flag(&instance->SR, SPI_SR_RXP))
        {
            return false;
        }
        byte = read_rxdr8(instance);
    }

    return finish_transfer(instance);
}

bool HwSpi::write(std::span<uint8_t> tx_data)
{
    if (instance == nullptr)
    {
        return false;
    }

    if ((instance->CR1 & SPI_CR1_SPE) == 0u)
    {
        return false;
    }

    if (tx_data.empty())
    {
        return true;
    }

    if (tx_data.size() > 0xFFFFu)
    {
        return false;
    }

    if (!start_transfer(instance, static_cast<uint16_t>(tx_data.size())))
    {
        return false;
    }

    for (const auto& byte : tx_data)
    {
        if (!wait_for_flag(&instance->SR, SPI_SR_TXP))
        {
            return false;
        }
        write_txdr8(instance, byte);

        if (!wait_for_flag(&instance->SR, SPI_SR_RXP))
        {
            return false;
        }
        (void)read_rxdr8(instance);
    }

    return finish_transfer(instance);
}

bool HwSpi::seq_transfer(std::span<uint8_t> tx_data, std::span<uint8_t> rx_data)
{
    if (instance == nullptr)
    {
        return false;
    }

    if ((instance->CR1 & SPI_CR1_SPE) == 0u)
    {
        return false;
    }

    const size_t frame_count = tx_data.size() + rx_data.size();
    if (frame_count == 0u)
    {
        return true;
    }

    if (frame_count > 0xFFFFu)
    {
        return false;
    }

    if (!start_transfer(instance, static_cast<uint16_t>(frame_count)))
    {
        return false;
    }

    for (const auto& byte : tx_data)
    {
        if (!wait_for_flag(&instance->SR, SPI_SR_TXP))
        {
            return false;
        }
        write_txdr8(instance, byte);

        if (!wait_for_flag(&instance->SR, SPI_SR_RXP))
        {
            return false;
        }
        (void)read_rxdr8(instance);
    }

    for (auto& byte : rx_data)
    {
        if (!wait_for_flag(&instance->SR, SPI_SR_TXP))
        {
            return false;
        }
        write_txdr8(instance, 0x00u);

        if (!wait_for_flag(&instance->SR, SPI_SR_RXP))
        {
            return false;
        }
        byte = read_rxdr8(instance);
    }

    return finish_transfer(instance);
}

//SPI initialization function to configure SPI peripheral based on settings struct
bool HwSpi::init()
{
    if (instance == nullptr)
    {
        return false;
    }

    //NOTE TO SELF
    //static_cast to uint8_t and range checks to validate enum values before writing to registers
    if (static_cast<uint8_t>(settings.baudrate) > 7u)
    {
        return false;
    }
    if (static_cast<uint8_t>(settings.busmode) > 3u)
    {
        return false;
    }
    if (static_cast<uint8_t>(settings.order) > 1u)
    {
        return false;
    }
    if (static_cast<uint8_t>(settings.threshold) > 1u)
    {
        return false;
    }

    instance->CR1 &= ~SPI_CR1_SPE;

    instance->CFG2 &= ~(SPI_CFG2_MASTER | SPI_CFG2_LSBFRST | SPI_CFG2_CPHA |
                        SPI_CFG2_CPOL | SPI_CFG2_SSM);
    instance->CFG2 |= (SPI_CFG2_MASTER | SPI_CFG2_SSM);

    if (settings.order == SpiBitOrder::LSB)
    {
        instance->CFG2 |= SPI_CFG2_LSBFRST;
    }

    //Configure clock polarity and phase based on bus mode
    switch (settings.busmode)
    {
    case SpiBusMode::MODE1:
        break;
    case SpiBusMode::MODE2:
        instance->CFG2 |= SPI_CFG2_CPHA;
        break;
    case SpiBusMode::MODE3:
        instance->CFG2 |= SPI_CFG2_CPOL;
        break;
    case SpiBusMode::MODE4:
        instance->CFG2 |= (SPI_CFG2_CPOL | SPI_CFG2_CPHA);
        break;
    }

    SetReg(&instance->CFG1, static_cast<uint32_t>(settings.baudrate),
           SPI_CFG1_MBR_Pos, 3);

    // 8-bit SPI frame (DSIZE field value is frame_bits - 1).
    SetReg(&instance->CFG1, 7u, SPI_CFG1_DSIZE_Pos, 5);

    // Minimal FIFO threshold for simple polling transfers.
    SetReg(&instance->CFG1, 0u, SPI_CFG1_FTHLV_Pos, 4);

    instance->CR1 |= SPI_CR1_SSI;
    instance->CR1 |= SPI_CR1_SPE;

    return true;
}

}  // namespace Stmh7
}  // namespace LBR
