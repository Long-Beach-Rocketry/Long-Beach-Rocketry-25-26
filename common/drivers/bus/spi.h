/**
 * @file spi.h
 * @author Kent Hong
 * @brief SPI API
 * @date 2025-09-30
 * 
 */
#pragma once
#include <cstddef>
#include <cstdint>

namespace LBR
{
class Spi
{
public:
    virtual bool Read(uint8_t* rx_data, size_t buffer_len) = 0;
    virtual bool Write(uint8_t* tx_data, size_t buffer_len) = 0;
    virtual bool Transfer(uint8_t* tx_data, uint8_t* rx_data, size_t tx_len,
                          size_t rx_len) = 0;
    virtual ~Spi() = default;
};
}  // namespace LBR