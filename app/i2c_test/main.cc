#include <cstdlib>
#include <span>
#include "board.h"
#include "i2c.h"

using namespace LBR;

int main(int argc, char* argv[])
{
    board_init();
    Board hw = get_board();

    uint8_t dummy_addr = 0x0F;
    uint8_t val[1] = {0xAA};
    std::span<const uint8_t> write_val{val, 1};

    while (1)
    {
        hw.i2c.write(write_val, dummy_addr);

        for (volatile size_t i = 0; i < 1000000; i++);
    }

    return 0;
}