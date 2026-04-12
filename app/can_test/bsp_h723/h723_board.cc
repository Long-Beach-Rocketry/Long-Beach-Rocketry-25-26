#include "st_can.h"
#include "st_gpio.h"
#include "board.h"

namespace LBR
{

// Pins init for CAN peripheral

bool bsp_init()
{
    // Initialize GPIOs for CAN
    // Initialize CAN peripheral
    return true; 
}

Board& get_board()
{
    return board;
}
} // namespace LBR