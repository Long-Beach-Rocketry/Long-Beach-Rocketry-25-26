#pragma once
#include "gpio.h"
#include "sys_clock.h"
#include "usart.h"

namespace LBR
{
struct Board
{
    Usart& usart;
    Gpio& rx;
    Gpio& tx;
    Clock& clock;
};

bool bsp_init();
Board& get_board();
}  // namespace LBR