#pragma once
#include "gpio.h"
#include "usart.h"

extern std::span<uint8_t> tx_span;

extern std::span<uint8_t> rx_span;

struct Board
{
    Usart& usart;

    LBR::Gpio& rx;
    LBR::Gpio& tx;
};

bool BSP_Init();
Board& get_board(void);