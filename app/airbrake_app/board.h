/**
 * @file board.h
 * @brief Airbrake Board Interface
 * @author Joseph Chang
 */

#pragma once
#include "bmp390.h"
#include "bno055_imu.h"
#include "sys_clock.h"
#include "timebase.h"

// TODO: Delete this once the sensor manager is merged
class SensorMgr;

namespace LBR
{

struct Board
{
    Clock& clock;
    Timebase& timebase;
    Bno055& bno055;
    Bmp390& bmp390;
    SensorMgr& sensor_mgr;
};

bool board_init(void);
Board& get_board(void);

}  // namespace LBR