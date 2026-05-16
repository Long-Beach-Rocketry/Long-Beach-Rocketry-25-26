/**
 * @file st_crc.h
 * @brief Crc driver specifics for the stmh7.
 * @author Joseph Chang
 */

#pragma once

#include "crc.h"
#include "stm32h723xx.h"

namespace LBR
{
namespace Stmh7
{

/**
 * @note Used to control the reversal of the bit order of the output data.
 */
enum class CrcRevOut : uint8_t
{
    NO_REV = 0,
    REVERSED
};

/**
 * @brief Used to control the reversal of the bit order of the input data
 */
enum class CrcRevIn : uint8_t
{
    NO_REV = 0,
    BYTE,
    HALF_WORD,
    WORD
};

/**
 * @brief Used to set the size of the polynomial.
 */
enum class CrcPolySize : uint8_t
{
    BIT_SIZE_32 = 0,
    BIT_SIZE_16,
    BIT_SIZE_8,
    BIT_SIZE_7
};

/**
 * @brief Collection of control params to configure crc.
 */
struct StCrcSettings
{
    CrcRevOut reverse_out;
    CrcRevIn reverse_in;
    CrcPolySize poly_size;
};

/**
 * @brief Collection of settings, configurables, and base address.
 */
struct StCrcParams
{
    StCrcSettings settings;
    CRC_TypeDef* base_addr;  // I think there is only one actually
    uint32_t initial_crc;
    uint32_t generator_polynomial = 0x4C11DB7;  // default 0x4C11DB7
};

class HwCrc : public Crc
{

public:
    /**
     * @brief Hw Contructor
     * @param params_ struct of inital crc value and generator polynomial
     */
    explicit HwCrc(const StCrcParams& params_);

    bool init();

    /**
     * @brief Computes the checksum from some data
     * @note STM32H7 can take input sizes of 8, 16, and 32 bits
     * @param data
     * @param result
     * 
     * 
     * the dividend: the input data
     * the divisor: the generator polynomial, set in init
     * an initial CRC value, set in init
     */
    bool compute(std::span<const uint32_t> data, uint32_t& result) override;
    bool compute(std::span<const uint16_t> data, uint32_t& result) override;
    bool compute(std::span<const uint8_t> data, uint32_t& result) override;

    /**
     * @brief Checks if the data has been flipped by computing a crc from the data and
     *        comparing it to originally given crc.
     * @note STM32H7 can take input sizes of 8, 16, and 32 bits
     * @param data
     * @param crc
     * 
     */
    bool compare(const std::span<const uint32_t> data,
                 const uint32_t crc) override;
    bool compare(const std::span<const uint16_t> data,
                 const uint32_t crc) override;
    bool compare(const std::span<const uint8_t> data,
                 const uint32_t crc) override;

private:
    void load(uint8_t value);
    void load(uint16_t value);
    void load(uint32_t value);

    CRC_TypeDef* const base_addr;
    StCrcSettings settings;
    uint32_t initial_crc;
    uint32_t generator_polynomial;
};

}  // namespace Stmh7
}  // namespace LBR