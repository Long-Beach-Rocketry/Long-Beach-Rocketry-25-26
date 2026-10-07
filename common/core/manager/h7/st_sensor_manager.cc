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

RawSensorData HwSensorMgr::get_latest_sensors() const
{
    return raw_data;
}

FilteredSensorData HwSensorMgr::filter_data(RawSensorData raw_data) const
{
    return filtered_data;   
}

// TODO: This can maybe be done in the main loop instead
// bool HwSensorMgr::update_sensor_data()
// {
//     // TODO: Check if timebase has gone to at least 100 Hz 
//     if (timebase.elapsed_since_us() >= 1000000) 
//     {
//         return false;
//     }

//     /* Check if barometer exists */
//     if (!sensors.bmp390)
//     {
//         return false;
//     }

//     /* Check if imu exists */
//     if (!sensors.bno055)
//     {
//         return false;
//     }

//     return true;
// }

}  // Namespace Stmh7
}  // Namespace LBR