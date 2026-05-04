#include "st_crc.h"
#include "reg_helpers.h"

namespace LBR
{
namespace Stmh7
{

static constexpr uint8_t kCRC_CR_REV_OUT_BitWidth = 1;
static constexpr uint8_t kCRC_CR_REV_IN_BitWidth = 2;
static constexpr uint8_t kCRC_CR_POLYSIZE = 2;

HwCrc::HwCrc(const StCrcParams& params_)
    : base_addr{params_.base_addr},
      settings{params_.settings},
      inital_crc{params_.initial_crc},
      generator_polynomial{params_.generator_polynomial}
{
}

bool HwCrc::init()
{
    // REV OUT, REV IN, POLYSIZE
    SetReg(&base_addr->CR, uint32_t(settings.reverse_out), CRC_CR_REV_OUT_Pos,
           kCRC_CR_REV_OUT_BitWidth);

    SetReg(&base_addr->CR, uint32_t(settings.reverse_in), CRC_CR_REV_IN_Pos,
           kCRC_CR_REV_IN_BitWidth);

    SetReg(&base_addr->CR, uint32_t(settings.poly_size), CRC_CR_POLYSIZE_Pos,
           kCRC_CR_POLYSIZE);

    // Settings vals
    base_addr->INIT = settings.initial_crc;
    base_addr->POL = settings.generator_polynomial;

    // Reset
    base_addr->CR |= CRC_CR_RESET;

    return true;
}

bool HwCrc::compute(std::span<uint8_t> data, uint32_t& result)
{
    base_addr->CR |= CRC_CR_RESET;

    for (uint8_t byte : data)
    {
        // for 8-byte
        *reinterpret_cast<volatile uint8_t*>(&base_addr->DR) = byte;
    }

    result = base_addr->DR;
    return true;
}

}  // namespace Stmh7
}  // namespace LBR