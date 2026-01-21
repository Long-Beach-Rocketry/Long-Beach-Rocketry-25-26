#pragma once
#include <array>
#include "gpio.h"
#include "usart.h"


extern std::array<uint8_t, 7> rxb;

//extern std::array<uint8_t, 7> txb;

struct Board
{
    Usart& usart;

    LBR::Gpio& rx;
    LBR::Gpio& tx;
};

bool BSP_Init();
Board& get_board();