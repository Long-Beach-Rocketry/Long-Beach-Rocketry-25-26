/**
 * @file board.h
 * @brief CLI Board Interface
 * @author Joseph Chang
 */

#pragma once
#include "bmp390.h"
#include "bno055_imu.h"
#include "cli.h"
#include "gpio.h"
#include "ring_buffer.h"
#include "sys_clock.h"
#include "usart.h"

extern LBR::RingBuffer<char, 128> rxBuffer;
extern uint8_t rxb;
extern LBR::Cli cli;
extern bool update_flag;

namespace LBR
{

struct Board
{
    Usart& usart;
    Clock& clock;
    Gpio& led1;
#ifdef STM32H723xx
    Gpio& led2;
    Gpio& led3;
#endif
    Bno055& bno055;
    Bmp390& bmp390;
};

bool board_init(void);
Board& get_board(void);

}  // namespace LBR