#include <cstdlib>
#include <span>
#include "board.h"
#include "crc.h"

using namespace LBR;

// Use https://ninjacalc.mbedded.ninja/calculators/software/crc-calculator
// and configuration of crc to compare value
constexpr uint32_t kActualCrc = 0x86C8C832;
uint32_t result = 0;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    // Data formatted because of endianess
    const uint8_t data8[] = {0x01, 0x02, 0x03, 0x04};
    const uint16_t data16[] = {0x0201, 0x0403};
    const uint32_t data32[] = {0x04030201};

    while (1)
    {
        hw.crc.compare(std::span(data8), kActualCrc);
        for (volatile uint32_t i = 0; i < 1000000; i++);

        hw.crc.compare(std::span(data16), kActualCrc);
        for (volatile uint32_t i = 0; i < 1000000; i++);

        hw.crc.compare(std::span(data32), kActualCrc);
        for (volatile uint32_t i = 0; i < 1000000; i++);
    }

    return 0;
}