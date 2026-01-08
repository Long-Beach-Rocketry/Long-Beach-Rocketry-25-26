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

namespace LBR
{

struct Board
{
    I2c& i2c;
    Gpio& gpio;
    Bno055Data imu;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace LBR
