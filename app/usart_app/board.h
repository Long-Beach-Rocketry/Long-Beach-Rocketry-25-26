#pragma once
#include "gpio.h"
#include "usart_andrew.h"

extern uint8_t rxb;

namespace LBR
{
struct Board
{
    UsartAndrew& usart;

    Gpio& rx;
    Gpio& tx;
};

bool bsp_init();
Board& get_board();
}  // namespace LBR