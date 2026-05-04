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

enum class CrcRevOut : uint8_t
{
    NO_REV = 0,
    REVERSED
};

enum class CrcRevIn : uint8_t
{
    NO_REV = 0,
    BYTE,
    HALFWORD,
    WORD
};

enum class CrcPolySize : uint8_t
{
    BIT_SIZE_32 = 0,
    BIT_SIZE_16,
    BIT_SIZE_8,
    BIT_SIZE_7
};

struct StCrcSettings
{
    CrcRevOut reverse_out;
    CrcRevIn reverse_in;
    CrcPolySize poly_size;
};

struct StCrcParams
{
    StCrcSettings settings;
    CRC_TypeDef* base_addr;  // I think there is only one actually
    uint32_t initial_crc;
    uint32_t generator_polynomial;  // default 0x4C11DB7
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
     * the dividend: also called input data, abbreviated to “Input_Data”
     * the divisor: also called generator polynomial, abbreviated to “POLY”
     * an initial CRC value: abbreviated to “Initial_Crc”
     */
    bool compute(std::span<uint8_t> data);

private:
    CRC_TypeDef* const base_addr;
    StCrcSettings settings;
    uint32_t initial_crc;
    uint32_t generator_polynomial;
};

}  // namespace Stmh7
}  // namespace LBR