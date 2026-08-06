#include "inv_bmp390.h"

namespace LBR
{
bool InvBmp390::process_read(uint8_t opcode, const uint8_t reg_addr,
                             uint8_t dev_addr)
{
    (void)opcode;
    (void)reg_addr;
    (void)dev_addr;
    return false;
}

bool InvBmp390::get_8bit_addr(uint8_t& addr)
{
    addr = CHIP_ID;
    return true;
}
}