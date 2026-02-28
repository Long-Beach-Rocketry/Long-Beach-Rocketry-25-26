#pragma once
#include "gpio.h"
//do the cmake

namespace LBR
{

struct Board
{
    Gpio& led1;
#ifdef STM32H723xx
    Gpio& led2;
    Gpio& led3;
#endif
};

bool board_init(void);
Board& get_board(void);

}  // namespace LBR