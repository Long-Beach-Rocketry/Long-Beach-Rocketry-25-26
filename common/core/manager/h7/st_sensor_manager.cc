/**
 * @file st_sensor_manager.cc
 * @brief Implementation for the H7 Sensor Manager
 * @author Joseph Chang, Alex Pulido
 */
#include "st_sensor_manager.h"

namespace LBR
{
namespace Stmh7
{

// TODO: set timer obj
HwSensorMgr::HwSensorMgr(const StSensorMgrParams& params_)
    : sensors{params_.sensors},
      timebase{params_.timebase},
      ekf{params_.ekf}
{
}

bool HwSensorMgr::update(RawSensorData& out)
{
    bool any_ok = false;    // Tracks if at least one sensor read was successful

    if (sensors.bno055 && sensors.bno055->read_all(raw_data.imu))
    {
        any_ok = true;
    }

    // Bmp390 reads have no failure signal, so presence is treated as success
    if (sensors.bmp390)
    {
        raw_data.baro.press = sensors.bmp390->get_pressure();
        raw_data.baro.temp = sensors.bmp390->get_temperature();
        any_ok = true;
    }

    if (!any_ok)
    {
        return false;
    }

    out = raw_data;
    return true;
}

FilteredSensorData HwSensorMgr::filter_data(RawSensorData raw_data) const
{
    // Pass raw data through the EKF instance to get filtered data
    // filtered_data = ekf.filter(raw_data);
    // if needed, pack the output of the ekf into the filtered data structure depending on what the ekf type returns
    return filtered_data;
}

}  // Namespace Stmh7
}  // Namespace LBR