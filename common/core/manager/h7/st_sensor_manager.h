/**
 * @file st_sensor_manager.h
 * @brief H7 specific sensor manager class
 * @author Joseph Chang, Alex Pulido
 */

#pragma once
#include "sensor_manager.h"
#include "seqlock.h"
#include "timebase.h"

#include <chrono>
#include <cstdint>

namespace LBR
{
class Ekf;
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
    Timebase& timebase;
    StSensorMgrSensors sensors;
    Ekf* ekf;
    std::chrono::microseconds sample_period;  // Sensor poll period
};

class HwSensorMgr : public SensorMgr
{
public:
    /**
     * @brief Hw Contructor
     * @param params_ Struct containing references to the timebase, sensors, and EKF instance used by the sensor manager
     */
    explicit HwSensorMgr(const StSensorMgrParams& params_);

    /**
     * @brief Configure the managed timebase to the sample period and start it
     * @return False if the period is invalid or the timebase could not start
     */
    bool start() override;

    /**
     * @brief True once per sample period since the previous true result
     * @return False if start() has not succeeded or the period has not elapsed
     */
    bool period_elapsed() override;

    /**
     * @brief Polls the managed sensors, skipping any that are null or fail
     * @param out Filled with the latest readings from the sensors that responded
     * @return True if at least one sensor was read, false if none were available
     */
    bool update(RawSensorData& out) override;

    /**
     * @brief Filters the raw sensor data struct using the EKF instance provided to the sensor manager
     * @param raw_data The raw sensor data to filter
     * @return The filtered sensor data struct returned by the EKF
     */
    FilteredSensorData filter_data(RawSensorData raw_data) override;

    /**
     * @brief Copy out the latest raw sensor data published by update(); safe to call from any consumer
     * @param out Filled with a consistent snapshot on success
     * @return False if nothing has been published yet or the read kept overlapping a write
     */
    bool get_raw(RawSensorData& out) const override;

    /**
     * @brief Copy out the latest filtered sensor data published by filter_data(); safe to call from any consumer
     * @param out Filled with a consistent snapshot on success
     * @return False if nothing has been published yet or the read kept overlapping a write
     */
    bool get_filtered(FilteredSensorData& out) const override;

private:
    SeqLock<RawSensorData> raw_buffer;
    SeqLock<FilteredSensorData> filtered_buffer;
    RawSensorData raw_data;
    FilteredSensorData filtered_data;
    StSensorMgrSensors sensors;
    Timebase& timebase;
    Ekf* ekf;
    std::chrono::microseconds sample_period;
    uint64_t last_sample_us{0};
    bool running{false};
};

}  // namespace Stmh7

}  // namespace LBR