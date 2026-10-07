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

    std::array<RawSensorData, 2> raw_buffers{};
    std::array<FilteredSensorData, 2> filtered_buffers{};

    size_t raw_write_index = 0;
    size_t filtered_write_index = 0;

    while (1)
    {
        if (hw.timebase.elapsed_since_us(last_time_us) >= kPeriodUs)
        {
            last_time_us += kPeriodUs;

            // Poll sensors into the raw write slot
            RawSensorData& raw_write_buffer = raw_buffers[raw_write_index];
            hw.imu.read_all(raw_write_buffer.imu);
            raw_write_buffer.baro.press = hw.bmp390.get_pressure();
            raw_write_buffer.baro.temp = hw.bmp390.get_temperature();

            // The slot just written becomes the stable read slot
            const size_t raw_read_index = raw_write_index;
            raw_write_index ^= 1;   // Toggle between 0 and 1 for double buffering

            // Filter the stable raw sample into the filtered write slot
            filtered_buffers[filtered_write_index] = hw.sensor_mgr.filter_data(raw_buffers[raw_read_index]);

            const RawSensorData& raw = raw_buffers[raw_read_index];
            const FilteredSensorData& filtered = filtered_buffers[filtered_write_index];

            printf("RAW  t=%llu us\n", static_cast<unsigned long long>(last_time_us));
            printf("  accel: %f %f %f\n", raw.imu.accel.x, raw.imu.accel.y, raw.imu.accel.z);
            printf("  gyro:  %f %f %f\n", raw.imu.gyro.x, raw.imu.gyro.y, raw.imu.gyro.z);
            printf("  lin:   %f %f %f\n", raw.imu.linear_accel.x, raw.imu.linear_accel.y, raw.imu.linear_accel.z);
            printf("  grav:  %f %f %f\n", raw.imu.gravity.x, raw.imu.gravity.y, raw.imu.gravity.z);
            printf("  quat:  %f %f %f %f\n", raw.imu.quat.w, raw.imu.quat.x, raw.imu.quat.y, raw.imu.quat.z);
            printf("  baro:  press=%f temp=%f\n", raw.baro.press, raw.baro.temp);

            printf("FILTERED\n");
            // TODO: print filtered.ekf fields once Ekf::Output is defined

            const size_t filtered_read_index = filtered_write_index;
            filtered_write_index ^= 1;  // Toggle between 0 and 1 for double buffering
            (void)filtered_read_index;  // Suppress unused variable warning
        }
    }

    return 0;
}