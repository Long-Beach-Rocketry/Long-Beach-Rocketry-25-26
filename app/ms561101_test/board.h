#pragma once

#include "ms561101.h"

namespace LBR
{

struct Board
{
    I2c& i2c;
    Ms561101& barometer;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace LBR