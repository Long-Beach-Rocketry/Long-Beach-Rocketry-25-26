#pragma once
#include "i2c.h"

namespace LBR
{

struct Board
{
    I2c& i2c;
};

bool board_init(void);
Board& get_board(void);

}  // namespace LBR