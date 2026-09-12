/**
 * @file   sensor_manager.h
 * @brief  Generic class to manage sensors and get fresh sensor information
 * @author Joseph CHang
 */

#pragma once
#include "sensor_data.h"

namespace LBR
{

class SensorMgr
{
public:
    /**
     * @brief Returns the latest sensor information 
     * @param None
     * @return SensorData struct
     */
    virtual SensorData get_latest_sensors() const = 0;

    /**
     * @brief Updates the sensor data struct with the most up to date readings
     * @param None
     * @return True
     */
    virtual bool update_sensor_data() = 0;

    ~SensorMgr() = default;
};

}  // namespace LBR