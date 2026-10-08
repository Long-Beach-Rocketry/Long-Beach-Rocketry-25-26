/**
 * @file sensor_data.h
 * @brief Struct to contain sensor information
 * @author Joseph Chang, Alex Pulido
 */

#pragma once

// #include "ms5611.h"
#include "bmp390.h"
#include "bno055_imu.h"
#include "ekf.h"

namespace LBR
{

/**
 * @brief Compensated barometer readings
 */
struct Bmp390Data
{
    float press;  // Pascals
    float temp;   // degrees Celsius
};

/**
 * @brief Container for raw sensor data
 */
struct RawSensorData
{
    // Ms5611Data baro;
    Bmp390Data baro;
    Bno055Data imu;
};

/**
 * @brief Container for filtered sensor data after Extended Kalman Filter
 */
struct FilteredSensorData
{
    // Ekf ekf;    // the filtered data the EKF will carry when it finishes through 
                   // (may need to pack it into this struct if it returns individual values)
};

}  // namespace LBR