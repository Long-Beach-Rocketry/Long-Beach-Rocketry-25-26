#pragma once
#include "bmp390.h"
#include "bno055_imu.h"
#include "i2c.h"
#include "timebase.h"

namespace LBR 
{

struct Board
{
    Bmp390& bmp390;
    Bno055& imu;
    Timebase& timebase;
    I2c& i2c;
};

bool bsp_init();
Board& get_board();

}   // namespace LBR