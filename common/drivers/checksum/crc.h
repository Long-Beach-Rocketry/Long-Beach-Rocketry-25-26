/**
 * @file crc.h
 * @brief Cyclical Redundancy Check driver interface.
 * @author Joseph Chang
 * @date 26/04/2026
 *
 */

#pragma once
#include <cstdint>
#include <span>

namespace LBR
{

class Crc
{
public:
    /**
   * @brief Compute checksum from data
   * 
   * @param data block of memory to compute on
   * @return true if successful, false otherwise
   */
    virtual bool compute(std::span<uint8_t> data) = 0;
    // virtual bool compute(std::span<uint16_t> data) = 0;
    // Idk what sizes we use yet, I do uint8_t for now
    ~Crc() = default;
};
}  // namespace LBR