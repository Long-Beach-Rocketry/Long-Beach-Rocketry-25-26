/**
* @file imu_app_bsp.h
* @brief Board Support Package for IMU application.
* @author Bex Saw
* @date 2025-10-24
*/

#pragma once
#include "bno055_imu.h"

namespace LBR
{

Bno055& bsp_init_imu();

}