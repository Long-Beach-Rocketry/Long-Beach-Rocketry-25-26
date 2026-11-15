/**
 * @file   board.h
 * @brief  Board definition for sensor manager test application
 * @author Alex Pulido
 */

#pragma once
#include "bno055_imu.h"
#include "i2c.h"
#include "ms561101.h"
#include "sensor_manager.h"
#include "timebase.h"

namespace LBR
{

struct Board
{
    Ms561101& ms561101;
    Bno055& imu;
    Timebase& timebase;
    I2c& i2c;
    SensorMgr& sensor_mgr;
};

bool bsp_init();
Board& get_board();

}  // namespace LBR