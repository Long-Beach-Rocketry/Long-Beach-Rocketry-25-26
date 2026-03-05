<<<<<<< HEAD
#pragma once
#include "gpio_cs.h"
#include "spi.h"

//do the cmake

namespace LBR
{

struct Board
{
    GpioChipSelect& cs;
    Spi& spi1;
};

bool bsp_init();
Board& get_board();

=======
#pragma once
#include "gpio_cs.h"
#include "spi.h"
#include "stm32l476xx.h"

//do the cmake

namespace LBR
{

struct Board
{
    GpioChipSelect& cs;
    Spi& spi1;
};

bool BSP_Init();
Board& Get_Board();

>>>>>>> 319d772 (ekf)
}  // namespace LBR