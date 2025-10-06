#pragma once
#include "gpio.h"
//do the cmake

namespace LBR
{

struct Board
{
    Gpio& led;
};

bool board_init(void);
Board& get_board(void);

}  // namespace LBR