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
   * @note F1, F2, F4, L1 | F0, F3, F7, G0, G4, H7, L0, L4, L4+, U5, WB, WL
   * Data bits size:   32 | 8, 16, 32 
   * Poly bits size:   32 | 7, 8, 16, 32     
   * 
   * @param data block of memory to compute crc on
   * @param result block of memory to store result in
   * @return true if successful, false otherwise
   */
    virtual bool compute(std::span<const uint32_t> data, uint32_t& result) = 0;
    virtual bool compute(std::span<const uint16_t> data, uint32_t& result) = 0;
    virtual bool compute(std::span<const uint8_t> data, uint32_t& result) = 0;
    // For boards that do not support 8 and 16 bit input, return false.

    /**
     * @brief Given data and crc value, compute on data and compare to given crc value
     * 
     * @param data
     * @param crc
     * @return true if given crc and computed crc are the same, false otherwise 
     */
    virtual bool compare(const std::span<const uint32_t> data,
                         const uint32_t crc) = 0;
    virtual bool compare(const std::span<const uint16_t> data,
                         const uint32_t crc) = 0;
    virtual bool compare(const std::span<const uint8_t> data,
                         const uint32_t crc) = 0;
    // For boards that do not support 8 and 16 bit input, return false.

    ~Crc() = default;
};
}  // namespace LBR