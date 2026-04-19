/**
* @file board.h
* @author Bex Saw
* @brief Board-specific definitions and initialization for the nanopb test application.
* @version 0.1
*/

#pragma once
#include "gpio.h"
#include "usart.h"

extern uint8_t rxb;

namespace LBR
{
struct Board
{
    Usart& usart;
    Gpio& rx;
    Gpio& tx;
};

bool bsp_init();
Board& get_board();
}  // namespace LBR