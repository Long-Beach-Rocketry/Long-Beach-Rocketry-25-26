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
    virtual bool Read(std::span<uint8_t> rx_data) = 0;
    virtual bool Write(std::span<uint8_t> tx_data) = 0;
    virtual bool Transfer(std::span<uint8_t> tx_data,
                          std::span<uint8_t> rx_data) = 0;
    virtual ~Spi() = default;
};
}  // namespace LBR