#pragma once
#include "i2c.h"

namespace LBR
{

struct Board
{
    I2c& i2c;
};

Board& board_init(void);

}  // namespace LBR