#include <cstdlib>
#include <span>
#include "board.h"
#include "crc.h"
#include "delay.h"

using namespace LBR;

// Use https://ninjacalc.mbedded.ninja/calculators/software/crc-calculator
// and configuration of crc to compare value

// Website ^ changed so I use this.
// https://crccalc.com/?crc=0A0B0C0D&method=CRC-32&datatype=hex&outtype=hex
// CRC standard used is CRC-32/BZIP2.
constexpr uint32_t kActualCrc_1 = 0x86C8C832;
constexpr uint32_t kActualCrc_2 = 0x8F06E7CE;

std::array<uint8_t, 8> txb8{"1234567"};
std::array<uint8_t, 16> txb16{"1234567 1234567"};
std::array<uint8_t, 32> txb32{"1234567 1234567 1234567 1234567"};

std::vector<std::span<uint8_t>> packets = {txb8, txb16, txb32};

uint8_t rxb;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    // Input data
    const uint8_t data8_1[] = {0x01, 0x02, 0x03, 0x04};
    const uint16_t data16_1[] = {0x0102, 0x0304};
    const uint32_t data32_1[] = {0x01020304};

    const uint8_t data8_2[] = {0x0A, 0x0B, 0x0C, 0x0D};
    const uint16_t data16_2[] = {0x0A0B, 0x0C0D};
    const uint32_t data32_2[] = {0x0A0B0C0D};

    /* uint8_t test */

    hw.crc.compare(std::span(data8_1), kActualCrc_1);
    hw.usart.send(data8_1);
    for (volatile uint32_t i = 0; i < 100000; i++);

    hw.crc.compare(std::span(data8_2), kActualCrc_2);
    hw.usart.send(data8_2);
    for (volatile uint32_t i = 0; i < 100000; i++);

    /* uint16_t test */

    hw.crc.compare(std::span(data16_1), kActualCrc_1);
    for (volatile uint32_t i = 0; i < 100000; i++);

    hw.crc.compare(std::span(data16_2), kActualCrc_2);
    for (volatile uint32_t i = 0; i < 100000; i++);

    /* uint32_t test */

    hw.crc.compare(std::span(data32_1), kActualCrc_1);
    for (volatile uint32_t i = 0; i < 100000; i++);

    hw.crc.compare(std::span(data32_2), kActualCrc_2);
    for (volatile uint32_t i = 0; i < 100000; i++);

    while (1)
    {
        for (auto packet : packets)
        {

            uint32_t crc_result;
            hw.crc.compute(packet, crc_result);

            auto string = simulate_noise(packet, 0.01, 0.01);

            hw.usart.send(string);

            // Busy wait
            for (volatile uint32_t i = 0; i < 1000000; i++);

            std::array<uint8_t, 20> resultb{"\r\nCrc result: "};
            hw.usart.send(resultb);

            // Busy wait
            for (volatile uint32_t i = 0; i < 1000000; i++);

            std::array<uint8_t, 1> crcb{hw.crc.compare(string, crc_result) ? 'o'
                                                                           : 'x'

            };
            hw.usart.send(crcb);

            // Busy wait
            for (volatile uint32_t i = 0; i < 1000000; i++);

            std::array<uint8_t, 4> endb{"\r\n"};
            hw.usart.send(endb);

            // Busy wait
            for (volatile uint32_t i = 0; i < 1000000; i++);
        }
    }

    return 0;
}