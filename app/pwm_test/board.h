/**
 * @file board.h
 * @brief BSP interface for PWM testing
 * @author Yshi Blanco
 * @date 01/11/2026
 */

#pragma once
#include "pwm.h"

namespace LBR
{

struct Board
{
    Pwm& pwm;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace LBR