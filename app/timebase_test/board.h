/**
* @file board.h
* @brief Board configuration for STM32H723 boards
* @author Bex Saw
* @date 7/17/2026
*/

#pragma once
#include "gpio.h"
#include "sys_clock.h"
#include "timebase.h"
#include "usart.h"

namespace LBR
{

struct Board
{
    Usart& usart;
    Clock& clock;
    Timebase& timebase;
    Gpio& led;
};

bool bsp_init();
Board& get_board();
}  // namespace LBR