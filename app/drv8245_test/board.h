/**
 * @file board.h
 * @brief Motor Support interface
 * @author Yshi Blanco
 * @date 01/09/2026
 */

#pragma once
//#include "pwm.h"

namespace LBR
{

struct Board
{
    Drv8245& drv8245;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace LBR