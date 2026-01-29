/**
 * @file board.h
 * @brief Board definition for IMU Test Application
 * @author Bex Saw
 * @date 2025-10-21
 */

#pragma once
#include "bno055_imu.h"

namespace LBR
{

struct Board
{
    Bno055& imu;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace LBR
