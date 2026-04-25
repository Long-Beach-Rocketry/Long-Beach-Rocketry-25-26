/**
 * @file board.h
 * @brief BSP interface for DRV8245 testing
 * @author Bex Saw
 * @date 4/24/2026
 */

#pragma once
#include "drv8245.h"
#include "gpio.h"
#include "pwm.h"

namespace LBR
{

struct Board
{
    Pwm& pwm;
    Gpio& dir;
    Drv8245 drv8245{Drv8245::DrvParams{dir, pwm}};
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace LBR