#include <cstdlib>
#include <span>
#include "board.h"
#include "i2c.h"

using namespace LBR;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    uint8_t dev_addr = 0x76;  // BMP390 device address

    const uint8_t reg_addr = 0x00;  // reg addr for chip ID

    uint8_t read_val = 0;
    std::span<uint8_t, 1> chip_id{&read_val, 1};

    while (1)
    {
        hw.i2c.mem_read(chip_id, reg_addr, dev_addr);

        for (volatile size_t i = 0; i < 100000; i++);
    }

    return 0;
}