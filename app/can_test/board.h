/**
 * @file board.h
 * @brief CAN driver interface
 * @author Bex Saw
 * @date 4/11/2026
 */

#pragma once
#include "can.h"

namespace LBR
{

struct Board
{
    Can& can;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace LBR