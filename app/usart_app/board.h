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

bool BspInit();
Board& GetBoard();
}  // namespace LBR