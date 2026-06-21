/**
 * @file sensor_data.h
 * @brief Struct to contain sensor information
 * @author Joseph Chang
 */

#pragma once
#include "bmp390.h"
#include "bno055_imu.h"

namespace LBR
{

/**
 * @brief Container for sensor data
 */
struct SensorData
{
    Bno055Data imu;
    float pressure;
    float temperature;
};

}  // namespace LBR