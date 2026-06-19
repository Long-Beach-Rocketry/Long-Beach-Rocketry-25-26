/**
* @file board.h
* @author Bex Saw
* @brief Board-specific definitions and initialization for the usart_pipe_test application.
* @version 0.1
*/

#pragma once
#include "crc.h"
#include "gpio.h"
#include "usart.h"
#include "usart_pipe.h"

inline uint8_t rxb = 0;

namespace LBR
{
struct Board
{
    Usart& usart;
    Gpio& rx;
    Gpio& tx;
    // Rs485& rs485; // TODO: Add RS485 control pin if needed
    Crc& crc;
    Pipeline& pipeline;
};

bool bsp_init();
Board& get_board();

// For testing purpose (remove when it's work)
uint32_t get_us();
}  // namespace LBR