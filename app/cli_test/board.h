/**
 * @file board.h
 * @brief CLI Board Interface
 */

#pragma once
#include "gpio.h"
#include "ring_buffer.h"
#include "usart.h"

extern LBR::RingBuffer<char, 128> rxBuffer;
extern uint8_t rxb;

namespace LBR
{

struct Board
{
    Usart& usart;
    Gpio& led1;
#ifdef STM32H723xx
    Gpio& led2;
    Gpio& led3;
#endif
};

bool board_init(void);
Board& get_board(void);

}  // namespace LBR