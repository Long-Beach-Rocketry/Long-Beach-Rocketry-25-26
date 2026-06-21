/**
 * @file st_sensor_manager.cc
 * @brief Implementation for the H7 Sensor Manager
 */

#pragma once
#include "st_sensor_manager.h"

namespace LBR
{
namespace Stmh7
{

// TODO: set timer obj
HwSensorMgr::HwSensorMgr(const StSensorMgrParams& params_) : sensors{params_.sensors}
{
}

SensorData HwSensorMgr::get_latest_sensors() const
{
    return data;
}
bool HwSensorMgr::update_sensor_data()
{
    /* Check if barometer exists and fetch data */
    if (sensors.bmp390)
    {
        data.pressure = sensors.bmp390->get_pressure();
        data.temperature = sensors.bmp390->get_temperature();
    }

    /* Check if imu exists and fetch data */
    if (sensors.bno055)
    {
        // Not sure why it can return false but maybe sensor unhealthy
        if (!sensors.bno055->read_all(data.imu))
        {
            return false;
        }
    }

    return true;
}

}  // Namespace Stmh7
}  // Namespace LBR