#pragma once
#include "gpio.h"
#include "spi.h"
#include "stm32l476xx.h"
//do the cmake

namespace LBR
{

struct Board
{
    Gpio& cs;
    Spi& spi1;
};

bool BSP_Init();
Board& Get_Board();

}  // namespace LBR