#include "board.h"
#include "sensor_data.h"
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>

using namespace LBR;
using namespace std::chrono_literals;

static constexpr uint64_t kPeriodUs{10'000};  // 10 ms (100 Hz)

int main(void)
{
    bsp_init();

    Board& hw = get_board();

    hw.timebase.set_period(10ms);
    hw.timebase.start();

    uint64_t last_time_us = hw.timebase.uptime_us();

    RawSensorData raw{};
    FilteredSensorData filtered{};

    while (1)
    {
        if (hw.timebase.elapsed_since_us(last_time_us) >= kPeriodUs)
        {
            last_time_us += kPeriodUs;

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
            (void)filtered;  // Suppress unused variable warning until filtered data is printed

            printf("RAW  t=%llu us\n", static_cast<unsigned long long>(last_time_us));
            printf("  accel: %f %f %f\n", raw.imu.accel.x, raw.imu.accel.y, raw.imu.accel.z);
            printf("  gyro:  %f %f %f\n", raw.imu.gyro.x, raw.imu.gyro.y, raw.imu.gyro.z);
            printf("  lin:   %f %f %f\n", raw.imu.linear_accel.x, raw.imu.linear_accel.y, raw.imu.linear_accel.z);
            printf("  grav:  %f %f %f\n", raw.imu.gravity.x, raw.imu.gravity.y, raw.imu.gravity.z);
            printf("  quat:  %f %f %f %f\n", raw.imu.quat.w, raw.imu.quat.x, raw.imu.quat.y, raw.imu.quat.z);
            printf("  baro:  press=%f temp=%f\n", raw.baro.press, raw.baro.temp);

            printf("FILTERED\n");
            // TODO: print filtered.ekf fields once Ekf's output is defined
        }
    }

    return 0;
}