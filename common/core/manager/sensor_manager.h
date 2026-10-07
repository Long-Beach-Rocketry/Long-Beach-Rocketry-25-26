/**
 * @file   sensor_manager.h
 * @brief  Generic class to manage sensors and get fresh sensor information
 * @author Joseph Chang, Alex Pulido
 */

#pragma once
#include "sensor_data.h"

namespace LBR
{

class SensorMgr
{
public:
    /**
     * @brief Returns the latest raw sensor information 
     * @param None
     * @return RawSensorData struct
     */
    virtual RawSensorData get_latest_sensors() const = 0;

    // /**
    //  * @brief Updates the sensor data struct with the most up to date readings
    //  * @param None
    //  * @return True if the sensor data was successfully updated, false otherwise
    //  */
    // virtual bool update_sensor_data() = 0;

    /**
     * @brief Filter the data through the EKF
     * @param raw_data The raw sensor data to filter
     * @return The filtered sensor data struct
     */
    virtual FilteredSensorData filter_data(RawSensorData raw_data) const = 0;

    ~SensorMgr() = default;
};

}  // namespace LBR