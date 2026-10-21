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

HwSensorMgr::HwSensorMgr(const StSensorMgrParams& params_)
    : sensors{params_.sensors},
      timebase{params_.timebase},
      ekf{params_.ekf},
      sample_period{params_.sample_period}
{
}

bool HwSensorMgr::start()
{
    if (sample_period.count() <= 0)
    {
        return false;
    }

    if (!timebase.set_period(sample_period) || !timebase.start())
    {
        return false;
    }

    // Record the current time as the last sample time to start the sampling window
    last_sample_us = timebase.uptime_us();
    running = true;
    return true;
}

bool HwSensorMgr::period_elapsed()
{
    if (!running)
    {
        return false;
    }

    const uint64_t period_us = static_cast<uint64_t>(sample_period.count());

    // Check if the required sample period has elapsed since the last sample
    if (timebase.elapsed_since_us(last_sample_us) < period_us)
    {
        return false;
    }

    // Advance by one period so a late caller catches up instead of drifting.
    last_sample_us += period_us;
    return true;
}

bool HwSensorMgr::update(RawSensorData& out)
{
    bool any_ok = false;  // Tracks if at least one sensor read was successful

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

    raw_buffer.write(raw_data);
    out = raw_data;
    return true;
}

FilteredSensorData HwSensorMgr::filter_data(RawSensorData raw_data)
{
    // Pass raw data through the EKF instance to get filtered data
    // filtered_data = ekf.filter(raw_data);
    // if needed, pack the output of the ekf into the filtered data structure depending on what the ekf type returns
    filtered_buffer.write(filtered_data);
    return filtered_data;
}

bool HwSensorMgr::get_raw(RawSensorData& out) const
{
    return raw_buffer.read(out);
}

bool HwSensorMgr::get_filtered(FilteredSensorData& out) const
{
    return filtered_buffer.read(out);
}

}  // Namespace Stmh7
}  // Namespace LBR