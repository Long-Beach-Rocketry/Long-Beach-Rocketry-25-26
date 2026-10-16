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
     * @brief Polls the sensors and updates the provided raw sensor data structure with the latest readings
     * @param out Filled with the latest readings from the sensors that responded
     * @return True if at least one sensor was read, false if none were available
     */
    virtual bool update(RawSensorData& out) = 0;

    /**
     * @brief Filter the raw sensor data through the EKF
     * @param raw_data The raw sensor data to filter
     * @return The filtered sensor data struct
     */
    virtual FilteredSensorData filter_data(RawSensorData raw_data) = 0;

    /**
     * @brief Copy out the latest raw sensor data published by update(); safe to call from any consumer
     * @param out Filled with a consistent snapshot on success
     * @return False if nothing has been published yet or the read kept overlapping a write
     */
    virtual bool get_raw(RawSensorData& out) const = 0;

    /**
     * @brief Copy out the latest filtered sensor data published by filter_data(); safe to call from any consumer
     * @param out Filled with a consistent snapshot on success
     * @return False if nothing has been published yet or the read kept overlapping a write
     */
    virtual bool get_filtered(FilteredSensorData& out) const = 0;

    ~SensorMgr() = default;
};

}  // namespace LBR