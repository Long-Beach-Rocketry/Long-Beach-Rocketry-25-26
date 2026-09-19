/**
 * @file board.h
 * @brief SYSCLK Board Interface
 * @author Kent Hong
 */

#pragma once
#include "sys_clock.h"
#include "usart.h"

extern uint8_t rxb;

namespace LBR
{

struct Board
{
    Usart& usart;
    Clock& clock;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace LBR