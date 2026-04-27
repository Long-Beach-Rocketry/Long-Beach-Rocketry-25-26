#include "telemetry.h"

namespace LBR
{

static uint32_t message_counter = 0;

static void map_imu_data(IMUData& pb_imu, const Bno055Data& sensor_imu)
{
    pb_imu.accel_x = sensor_imu.accel.x;
    pb_imu.accel_y = sensor_imu.accel.y;
    pb_imu.accel_z = sensor_imu.accel.z;
    pb_imu.gyro_x = sensor_imu.gyro.x;
    pb_imu.gyro_y = sensor_imu.gyro.y;
    pb_imu.gyro_z = sensor_imu.gyro.z;
    pb_imu.quat_w = sensor_imu.quat.w;
    pb_imu.quat_x = sensor_imu.quat.x;
    pb_imu.quat_y = sensor_imu.quat.y;
    pb_imu.quat_z = sensor_imu.quat.z;
}

PbCmd TelemetryPacket::send_telemetry(Bno055& imu, Bmp390& baro)
{
    PbCmd msg;
    Bmp390Data baro_data;
    Bno055Data imu_data;

    // Initialize the telemetry payload
    msg.payload.telemetry = Telemetry_init_default;

    // Fill header
    msg.payload.telemetry.header.device_id = 0x67;  // This board is ID 67
    msg.payload.telemetry.header.packet_count = message_counter++;
    msg.payload.telemetry.header.timestamp_ms =
        0;  // replace with actual millis()

    // Read the data directly from sensors!
    if (imu.read_all(imu_data))
    {
        map_imu_data(msg.payload.telemetry.imu, imu_data);
    }

    // Read Baro
    if (baro.get_pressure(baro_data.pressure) &&
        baro.get_temperature(baro_data.temperature))
    {
        msg.payload.telemetry.baro.pressure_pa = baro_data.pressure;
        msg.payload.telemetry.baro.temperature = baro_data.temperature;
    }

    return msg;
}
}  // namespace LBR
