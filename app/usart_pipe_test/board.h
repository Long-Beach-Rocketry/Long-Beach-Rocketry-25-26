/**
* @file board.h
* @author Bex Saw
* @brief Board-specific definitions and initialization for the usart_pipe_test application.
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
    // Rs485& rs485; // TODO: Add RS485 control pin if needed
};

bool bsp_init();
Board& get_board();
}  // namespace LBR