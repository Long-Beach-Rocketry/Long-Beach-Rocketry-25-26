#pragma once
#include "gpio.h"
#include "usart.h"

extern uint8_t rxb;

extern uint8_t txb[];

struct Board
{
    Usart& usart;

    LBR::Gpio& rx;
    LBR::Gpio& tx;
};

bool BSP_Init();
Board& get_board();