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
    virtual bool Read();
    virtual bool Write();
    virtual bool Transfer();
    ~Spi() = default;
};
}  // namespace LBR