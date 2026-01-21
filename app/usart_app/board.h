#pragma once
#include <array>
#include "gpio.h"
#include "usart.h"

extern uint8_t rxb;

namespace LBR
{
struct Board
{
    Usart& usart;

    LBR::Gpio& rx;
    LBR::Gpio& tx;
};

bool BSP_Init();
Board& get_board();
}  // namespace LBR