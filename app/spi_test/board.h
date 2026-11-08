#pragma once
#include "gpio_cs.h"
#include "spi.h"
#include "sys_clock.h"

//do the cmake

namespace LBR
{

struct Board
{
    GpioChipSelect& cs;
    Spi& spi;
    Clock& clock;
};

bool bsp_init();
Board& get_board();

}  // namespace LBR