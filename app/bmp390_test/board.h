/**
 * @file board.h
 * @brief BSP interface for BMP390 testing
 * @author Yshi Blanco
 * @date 11/25/2025
 */

#pragma once
#include "bmp390.h"

namespace LBR
{

struct Board
{
    Bmp390& bmp390;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace LBR