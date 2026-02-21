<<<<<<< HEAD
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
=======
/**
 * @file board.h
 * @brief IMU test board interface
 * @author Bex Saw
 * @date 10/21/2025
 */

#pragma once
#include "bno055_imu.h"
#include "delay.h"
#include "gpio.h"
#include "i2c.h"

namespace LBR
{

struct Board
{
    Bno055& imu;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace LBR
>>>>>>> 319d772 (ekf)
