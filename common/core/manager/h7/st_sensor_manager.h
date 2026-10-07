/**
 * @file st_sensor_manager.h
 * @brief H7 specific sensor manager class
 * @author Joseph Chang, Alex Pulido
 */

#pragma once
#include "sensor_manager.h"
#include "timebase.h"

namespace LBR
{
namespace Stmh7
{

/**
 * @brief Sensors that can be passed in managed by the Sensor Manager.
 * Ptr's because they may be optional or not exist.
 */
struct StSensorMgrSensors
{
    Bno055* const bno055;   // IMU
    Bmp390* const bmp390;   // Barometer (old)
    // Ms5611* const ms5611;   // Barometer
};

struct StSensorMgrParams
{
    Timebase* timebase;
    StSensorMgrSensors sensors;
    Ekf* ekf;
};

class HwSensorMgr : public SensorMgr
{
public:
    /**
     * @brief Hw Contructor
     * @param params_ struct of pointer to timebase, sensors, and EKF instance
     */
    explicit HwSensorMgr(const StSensorMgrParams& params_);

    /**
     * @brief Simple getter of the up to date raw sensor data 
     * @param None
     * @return RawSensorData struct
     */
    RawSensorData get_latest_sensors() const override;

    // /**
    //  * @brief Gets the readings of data from all passed in sensors and updates SensorData
    //  * @param None
    //  * @return True if successful, false if something unexpected occurs
    //  */
    // bool update_sensor_data() override;

    /**
     * @brief Filters the raw sensor data struct using the EKF instance provided to the sensor manager
     * @param raw_data The raw sensor data to filter
     * @return The filtered sensor data struct returned by the EKF
     */
    FilteredSensorData filter_data(RawSensorData raw_data) const override;

private:
    RawSensorData raw_data;
    FilteredSensorData filtered_data;
    StSensorMgrSensors sensors;
    Timebase* timebase;
    Ekf* ekf;
};

}  // namespace Stmh7

}  // namespace LBR