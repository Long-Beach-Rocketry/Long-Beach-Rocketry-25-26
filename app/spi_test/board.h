#pragma once
#include "gpio_cs.h"
#include "spi.h"

#ifndef NATIVE
#include "stm32l476xx.h"
#endif

//do the cmake

namespace LBR
{

struct Board
{
    GpioChipSelect& cs;
    Spi& spi1;
};

bool BspInit();
Board& GetBoard();

}  // namespace LBR