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
    PbCmd cmd;
    cmd.msg = RocketMessage_init_default;

    cmd.msg.has_header = true;
    cmd.msg.header.device_id = 0x67;
    cmd.msg.header.packet_count = message_counter++;
    cmd.msg.header.timestamp_ms = 0;  // TODO: replace with actual millis()

    Bno055Data imu_data;
    if (imu.read_all(imu_data))
    {
        cmd.msg.has_imu = true;
        map_imu_data(cmd.msg.imu, imu_data);
    }

    cmd.msg.has_baro = true;
    cmd.msg.baro.pressure = baro.get_pressure();
    cmd.msg.baro.temperature = baro.get_temperature();

    return cmd;
}
}  // namespace LBR
