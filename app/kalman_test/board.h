#pragma once
#include "bno055.h"
#include "bmp390"

namespace LBR
{

struct Board {
    Bno055 imu;
    Bmp390 bmp; 
}

bool board_init();
Board& get_board();
}