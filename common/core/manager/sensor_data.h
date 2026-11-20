/**
 * @file sensor_data.h
 * @brief Struct to contain sensor information
 * @author Joseph Chang, Alex Pulido
 */

#pragma once

#include "bno055_imu.h"
#include "ms561101.h"
// #include "ekf.h"

namespace LBR
{

/**
 * @brief Compensated barometer readings from the MS561101 sensor
 */
struct Ms561101Data
{
    float press;  // Pascals
    float temp;   // degrees Celsius
};

/**
 * @brief Container for raw sensor data
 */
struct RawSensorData
{
    Ms561101Data baro;
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