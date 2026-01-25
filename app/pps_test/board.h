/**
 * @file board.h
 * @brief PPS driver interface
 * @author Bex Saw
 * @date 1/13/2026
 */

#pragma once
#include "i2c.h"
#include "gpio.h"
#include "bno055_imu.h"
#include "drv8245.h"

namespace LBR
{

struct Board
{
    I2c& i2c;
    Gpio& gpio;
    Bno055Data imu;
    Motor* motor = nullptr; // Add pointer to Motor abstraction (dummy or real)
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace LBR
