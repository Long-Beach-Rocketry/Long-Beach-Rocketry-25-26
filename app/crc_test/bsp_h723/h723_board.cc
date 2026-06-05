#include <cstdint>
#include "board.h"
#include "st_crc.h"

namespace LBR
{

/**
 * @note We can configure the crc according to some crc standard/algorithm.
 * Use https://ninjacalc.mbedded.ninja/calculators/software/crc-calculator as 
 * reference to see what to configure and the proper output.
 * 
 * Or https://crccalc.com/?crc=12345678&method=&datatype=hex&outtype=hex
 * 
 * Current crc standard for the test: CRC-32/BZIP2
 */

static constexpr uint32_t kDefaultPoly{0x4C11DB7};
static constexpr uint32_t kCrcValue{CRC_INIT_INIT};
static constexpr uint32_t kCrcXOROut{0xFFFFFFFF};

// Crc config
Stmh7::StCrcSettings crc_settings{Stmh7::CrcRevOut::NO_REV,
                                  Stmh7::CrcRevIn::NO_REV,
                                  Stmh7::CrcPolySize::BIT_SIZE_32};

const Stmh7::StCrcParams crc_params{crc_settings, CRC, kCrcValue, kDefaultPoly,
                                    kCrcXOROut};

// Create Crc object
Stmh7::HwCrc crc(crc_params);

Board board{.crc = crc};

bool bsp_init()
{
    // Enable peripheral clocks
    RCC->AHB4ENR |= RCC_AHB4ENR_CRCEN;

    // Initialize Crc and pins
    bool ret = true;

    ret = ret && crc.init();

    return ret;
}

Board& get_board()
{
    return board;
}

}  // namespace LBR