/**
 * @file   main.cc
 * @brief  Test application for the sensor manager
 * @author Alex Pulido
 */

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include "board.h"
#include "sensor_data.h"

using namespace LBR;

int main(void)
{
    bsp_init();

    Board& hw = get_board();

    // Period and timebase start live on the sensor manager. Airbrake/MCP uses
    // period_elapsed() for the same gate instead of touching the timebase.
    if (!hw.sensor_mgr.start())
    {
        printf("Failed to start sensor manager\n");
        while (1)
        {
        }
    }

    RawSensorData raw{};
    FilteredSensorData filtered{};

    while (1)
    {
        if (hw.sensor_mgr.period_elapsed())
        {
            // Poll sensors (if at least one is available), which publishes to the manager's raw buffer
            RawSensorData polled{};
            if (!hw.sensor_mgr.update(polled))
            {
                printf("No sensors available\n");
                continue;
            }

            // Filtering publishes to the manager's filtered buffer
            hw.sensor_mgr.filter_data(polled);

            // Consumers pull the latest snapshots from the manager
            hw.sensor_mgr.get_raw(raw);
            hw.sensor_mgr.get_filtered(filtered);
            (void)
                filtered;  // Suppress unused variable warning until filtered data is printed

            printf("RAW  t=%llu us\n",
                   static_cast<unsigned long long>(hw.timebase.uptime_us()));
            printf("  accel: %f %f %f\n", raw.imu.accel.x, raw.imu.accel.y,
                   raw.imu.accel.z);
            printf("  gyro:  %f %f %f\n", raw.imu.gyro.x, raw.imu.gyro.y,
                   raw.imu.gyro.z);
            printf("  lin:   %f %f %f\n", raw.imu.linear_accel.x,
                   raw.imu.linear_accel.y, raw.imu.linear_accel.z);
            printf("  grav:  %f %f %f\n", raw.imu.gravity.x, raw.imu.gravity.y,
                   raw.imu.gravity.z);
            printf("  quat:  %f %f %f %f\n", raw.imu.quat.w, raw.imu.quat.x,
                   raw.imu.quat.y, raw.imu.quat.z);
            printf("  baro:  press=%f Pa, temp=%f°C\n", raw.baro.press,
                   raw.baro.temp);

            printf("FILTERED\n");
            // TODO: print filtered.ekf fields once Ekf's output is defined
        }
    }

    return 0;
}