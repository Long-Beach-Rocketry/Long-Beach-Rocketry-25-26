#pragma once
#include "usart.h"

struct Board
{
    Usart& usart;
};

bool board_init(void);
Board& get_board(void);