/**
 * @file fixed_point.h
 * @brief This file provides fixed point decimals.
 * @note Fixed point arithmatic is faster and more accurate then floating point.
 * @author TJ Malaska
 * @date 
 */

#pragma once
#include <cstdint>

namespace LBR
{
/**
 * @class fixed_point
 * @breif
 */
template <typename rawT, typename upperT, typename lowerT, int fraction_bits>
class fixed_point
{
    static_assert(std::is_integral_v<rawT>, "rawT must be integral");
    static_assert(std::is_integral_v<upperT>, "upperT must be integral");
    static_assert(std::is_integral_v<lowerT>, "lowerT must be integral");
    static_assert(fraction_bits > 0, "fraction_bits must be > 0");
    static_assert(fraction_bits < int(sizeof(rawT) * 8),
                  "fraction_bits too large for rawT");

public:
    /**
     * @brief Constructs fixed point intager with decimal
     * @example fixed8_8(10, 45) would = 10.45
     */
    fixed_point(upperT upper, lowerT lower)
    {
        raw{upper << fraction_bits};
        raw |= lower;
    }

    /**
     * @brief Constructs fixed point intager wholenumber
     * @example fixed8_8(10) would = 10.0
     */
    fixed_point(upperT upper)
    {
        raw{upper << fraction_bits};
    }

    /* Default constructor. */
    fixed_point()
    {
        raw{0};
    }

    rawT get_raw()
    {
        return raw;
    }

    void set_raw(rawT new_raw)
    {
        raw = new_raw;
    }

    float to_float()
    {
        return static_cast<float>(raw) /
               static_cast<float>(rawT{1} << fraction_bits);
    }

    fixed_point to_double()
    {
        return static_cast<double>(raw) /
               static_cast<double>(rawT{1} << fraction_bits);
    }

private:
    rawT raw;
};

using fixed32_32 = fixed_point<int64_t, int32_t, uint32_t, 32>;
using fixed16_16 = fixed_point<int32_t, int16_t, uint16_t, 16>;
using fixed8_8 = fixed_point<int16_t, int8_t, uint8_t, 8>;
}  // namespace LBR