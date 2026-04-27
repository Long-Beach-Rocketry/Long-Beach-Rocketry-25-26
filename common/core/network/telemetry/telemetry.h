/**
* @file telemetry.h
* @author Bex Saw
* @brief Telemetry struct definition for Board-To-Board communication. 
*        This is the message that will be sent from the main board to 
*        the telemetry board over USART-RS485-USART.
*/

#pragma once

#include "bmp390.h"
#include "bno055_imu.h"
#include "imu_math.h"
#include "pb_cmd.h"

namespace LBR
{
class TelemetryPacket
{

public:
    /**
     * @brief Header struct for the telemetry message
     */
    struct Header
    {
        uint8_t
            device_id;   // ID of the sending device (e.g., 0x01 for main board)
        uint8_t msg_id;  // Message ID for tracking different message types
        uint32_t uptime_ms;  // Uptime in milliseconds for timestamping
    } header;

    /**
     * @brief IMU data struct for the telemetry message
     */
    struct IMUData
    {
        Vec3 accel;
        Vec3 gyro;
        Quaternion quat;
    } imu;

    /**
     * @brief Barometer data struct for the telemetry message
     */
    struct BarometerData
    {
        float pressure;
        float temperature;
    } baro;

    /**
     * @brief Sends the telemetry message
     */
    void send_telemetry(Bno055Data& imu, Bmp390& baro);
};
}  // namespace LBR