/**
 * @file board.h
 * @brief IMU test board interface
 * @author Bex Saw
 * @date 10/21/2025
 */

#pragma once
#include "delay.h"
#include "gpio.h"
#include "i2c.h"
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