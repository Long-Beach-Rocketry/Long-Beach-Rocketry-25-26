#pragma once

#include "gpio.h"
#include "gpio_cs.h"
#include "spi.h"

namespace LBR
{

struct Board
{
    GpioChipSelect& cs;
    Spi& spi;
    Gpio& reset;
    Gpio& busy;
};

bool bsp_init();
Board& get_board();

}  // namespace LBR