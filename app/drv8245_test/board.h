/**
 * @file board.h
 * @brief BSP interface for DRV8245 testing
 * @author Bex Saw  
 * @date 1/15/2026
 */

#pragma once
#include "drv8245.h"
#include "gpio.h"
#include "pwm.h"

namespace LBR
{

struct Board
{
    Drv8245& motor;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace LBR