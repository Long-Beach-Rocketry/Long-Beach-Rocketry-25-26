/**
 * @file board.h
 * @brief I2C driver interface
 * @author Yshi Blanco
 * @date 10/21/2025
 */

#pragma once
#include "i2c.h"

namespace LBR
{

struct Board
{
    I2c& i2c;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace LBR