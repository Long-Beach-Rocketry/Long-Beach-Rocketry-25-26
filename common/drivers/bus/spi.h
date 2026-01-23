/**
 * @file spi.h
 * @author Kent Hong
 * @brief SPI API
 * @date 2025-09-30
 * 
 */
#pragma once
#include <cstdint>
#include <span>

namespace LBR
{
class Spi
{
public:
    virtual bool read(std::span<uint8_t> rx_data) = 0;
    virtual bool write(std::span<uint8_t> tx_data) = 0;
    virtual bool seq_transfer(std::span<uint8_t> tx_data,
                              std::span<uint8_t> rx_data) = 0;
    virtual ~Spi() = default;
};
}  // namespace LBR