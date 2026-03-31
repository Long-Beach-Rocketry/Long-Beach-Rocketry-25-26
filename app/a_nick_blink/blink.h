#pragma once
#include "gpio.h"

namespace LBR
{

struct Board
{
    Gpio& led;
};

bool board_init(void);
Board& get_board(void);

} 