/**
* @file board.h
* @brief BSP interface for DRV8874 testing
* @author Bex Saw
* @date 1/22/2026
*/

#pragma once
#include "drv8874.h"
#include "gpio.h"
#include "pwm.h"

namespace LBR 
{

struct Board 
{
    Drv8874 &motor;
};

bool bsp_init(void);
Board &get_board(void);

} // namespace LBR