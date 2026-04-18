#pragma once
#include "gpio_cs.h"
#include "spi.h"

//do the cmake

namespace LBR
{

struct Board
{
    GpioChipSelect& cs;
    Spi& spi2;
};

bool bsp_init();
Board& get_board();

}  // namespace LBR