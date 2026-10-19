#pragma once
#include "bno055_imu.h"
#include "bmp390.h"
#include "sys_clock.h"
#include "altitude.h"

namespace LBR
{

struct Board {
    Clock& clock;
    Bno055& bno055;
    Bmp390& bmp390; 
};

bool board_init();
Board& get_board();

}