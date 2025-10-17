/**
 * @file spi.h
 * @author Kent Hong
 * @brief SPI API
 * @date 2025-09-30
 * 
 */
#pragma once

namespace LBR
{
class Spi
{
public:
    virtual bool Read() = 0;
    virtual bool Write() = 0;
    virtual bool Transfer() = 0;
    virtual ~Spi() = default;
};
}  // namespace LBR