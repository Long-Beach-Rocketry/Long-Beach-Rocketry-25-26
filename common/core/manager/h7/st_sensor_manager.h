/**
 * @file st_sensor_manager.h
 * @brief H7 specific sensor manager class
 * @author Joseph Chang
 */

#pragma once
#include "sensor_manager.h"

namespace LBR
{
namespace Stmh7
{

/**
 * @brief Sensors that can be passed in managed by the Sensor Manager.
 * Ptr's because they may be optional or not exist.
 */
struct StSensorMgrParams
{
    Bno055* const bno055;
    Bmp390* const bmp390;
};

class HwSensorMgr : SensorMgr
{
public:
    /**
     * @brief Hw Contructor
     * @param params_ struct of pointer to sensors
     */
    explicit HwSensorMgr(const StSensorMgrParams& params_);

    /**
     * @brief Simple getter of the up to date sensor data 
     * @param None
     * @return SensorData struct
     */
    SensorData get_latest_sensors() const override;

    /**
     * @brief Gets the readings of data from all passed in sensors and updates SensorData
     * @param None
     * @return True if successful, false if something unexpected occurs
     */
    bool update_sensor_data() override;

private:
    SensorData data;
    StSensorMgrParams sensors;
};

}  // namespace Stmh7

}  // namespace LBR