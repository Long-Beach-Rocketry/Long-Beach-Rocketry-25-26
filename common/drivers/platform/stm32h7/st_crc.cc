#include "st_crc.h"
#include "reg_helpers.h"

namespace LBR
{
namespace Stmh7
{

static constexpr uint8_t kCRC_CR_REV_OUT_BitWidth = 1;
static constexpr uint8_t kCRC_CR_REV_IN_BitWidth = 2;
static constexpr uint8_t kCRC_CR_POLYSIZE_BitWidth = 2;

inline void HwCrc::load(uint8_t value)
{
    volatile uint8_t* dr8 = reinterpret_cast<volatile uint8_t*>(&base_addr->DR);
    *dr8 = value;
}
inline void HwCrc::load(uint16_t value)
{
    volatile uint16_t* dr16 =
        reinterpret_cast<volatile uint16_t*>(&base_addr->DR);
    *dr16 = value;
}
inline void HwCrc::load(uint32_t value)
{
    volatile uint32_t* dr32 =
        reinterpret_cast<volatile uint32_t*>(&base_addr->DR);
    *dr32 = value;
}

HwCrc::HwCrc(const StCrcParams& params_)
    : base_addr{params_.base_addr},
      settings{params_.settings},
      initial_crc{params_.initial_crc},
      generator_polynomial{params_.generator_polynomial},
      xor_out{params_.XOR_out}
{
}

bool HwCrc::init()
{
    // Initial CRC value
    base_addr->INIT = initial_crc;

    // REV OUT, REV IN, POLYSIZE
    SetReg(&base_addr->CR, uint32_t(settings.reverse_out), CRC_CR_REV_OUT_Pos,
           kCRC_CR_REV_OUT_BitWidth);

    SetReg(&base_addr->CR, uint32_t(settings.reverse_in), CRC_CR_REV_IN_Pos,
           kCRC_CR_REV_IN_BitWidth);

    SetReg(&base_addr->CR, uint32_t(settings.poly_size), CRC_CR_POLYSIZE_Pos,
           kCRC_CR_POLYSIZE_BitWidth);

    // Generator polynomial value
    base_addr->POL = generator_polynomial;

    // Reset
    base_addr->CR |= CRC_CR_RESET;

    return true;
}

bool HwCrc::compute(std::span<const uint32_t> data, uint32_t& result)
{
    // Reset before each computation
    base_addr->CR |= CRC_CR_RESET;

    for (auto word : data)
    {
        load(word);
    }

    result = base_addr->DR ^ xor_out;

    return true;
}

bool HwCrc::compute(std::span<const uint16_t> data, uint32_t& result)
{
    // Reset before each computation
    base_addr->CR |= CRC_CR_RESET;

    for (auto halfword : data)
    {
        load(halfword);
    }

    result = base_addr->DR ^ xor_out;

    return true;
}

bool HwCrc::compute(std::span<const uint8_t> data, uint32_t& result)
{
    // Reset before each computation
    base_addr->CR |= CRC_CR_RESET;

    for (auto byte : data)
    {
        load(byte);
    }

    result = base_addr->DR ^ xor_out;

    return true;
}

bool HwCrc::compare(const std::span<const uint32_t> data, const uint32_t crc)
{
    uint32_t expected_crc;

    if (compute(data, expected_crc))
    {
        return (expected_crc == crc);
    }
    return false;
}

bool HwCrc::compare(const std::span<const uint16_t> data, const uint32_t crc)
{
    uint32_t expected_crc;
    if (compute(data, expected_crc))
    {
        return (expected_crc == crc);
    }
    return false;
}

bool HwCrc::compare(const std::span<const uint8_t> data, const uint32_t crc)
{
    uint32_t expected_crc;
    if (compute(data, expected_crc))
    {
        return (expected_crc == crc);
    }
    return false;
}

}  // namespace Stmh7
}  // namespace LBR