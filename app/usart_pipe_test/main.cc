#include <cstdio>
#include "board.h"
#include "delay.h"
#include "usart_pipe.h"

using namespace LBR;
using namespace LBR::Utils;

/**
 * @note Loopback test for the USART pipeline with protobuf encoding/decoding.
 *       Requires PD8 (TX) wired to PD9 (RX) for loopback.
 *
 *       Flow: fill PbCmd → pipeline.send() frames + CRC32 → USART TX
 *             → loopback wire → USART RX → pipeline.receive() validates + decodes → PbCmd
 */

int main()
{
    bsp_init();
    Board board = get_board();

    PbCmd tx{};
    PbCmd rx{};
    uint32_t count = 0;

    while (1)
    {
        // Feed fake data
        tx.msg.has_status = true;
        tx.msg.status.fsw_status = FswStatus_FSW_STATUS_ACTIVE;
        tx.msg.status.rocket_state = RocketState_ROCKET_STATE_POWERED_FLIGHT;
        tx.msg.status.airbrake_status =
            AirbrakeStatus_AIRBRAKE_STATUS_RETRACTED;

        tx.msg.has_imu = true;
        tx.msg.imu.accel_x = 1.0f + (count * 0.1f);
        tx.msg.imu.accel_y = 2.0f;
        tx.msg.imu.accel_z = 9.8f;
        tx.msg.imu.gyro_x = 0.1f;
        tx.msg.imu.gyro_y = 0.2f;
        tx.msg.imu.gyro_z = 0.3f;
        tx.msg.imu.quat_x = 0.0f;
        tx.msg.imu.quat_y = 0.0f;
        tx.msg.imu.quat_z = 0.0f;
        tx.msg.imu.linear_accel_x = 0.0f;
        tx.msg.imu.linear_accel_y = 0.0f;
        tx.msg.imu.linear_accel_z = 0.0f;

        tx.msg.has_baro = true;
        tx.msg.baro.air_pressure = 101325.0f - (count * 10.0f);
        tx.msg.baro.altitude = 10.0f + (count * 1.5f);

        // Stamp real hardware time right at send
        tx.msg.timestamp_us = get_us();
        board.pipeline.send(&tx, board.usart);

        // Wait for all bytes to arrive back via loopback (~22ms at 115200 baud)
        DelayMs(50);

        bool ok = board.pipeline.receive(&rx, board.usart);

        if (ok)
        {
            printf(
                "[PASS] count=%lu ts=%luus\r\n"
                "  IMU   accel=[%.2f, %.2f, %.2f]  gyro=[%.3f, %.3f, %.3f]\r\n"
                "        quat=[%.3f, %.3f, %.3f]  lin=[%.3f, %.3f, %.3f]\r\n"
                "  BARO  press=%.1fPa  alt=%.2fm\r\n"
                "  STAT  fsw=%d  state=%d  ab=%d\r\n",
                (unsigned long)count, (unsigned long)rx.msg.timestamp_us,
                rx.msg.imu.accel_x, rx.msg.imu.accel_y, rx.msg.imu.accel_z,
                rx.msg.imu.gyro_x, rx.msg.imu.gyro_y, rx.msg.imu.gyro_z,
                rx.msg.imu.quat_x, rx.msg.imu.quat_y, rx.msg.imu.quat_z,
                rx.msg.imu.linear_accel_x, rx.msg.imu.linear_accel_y,
                rx.msg.imu.linear_accel_z, rx.msg.baro.air_pressure,
                rx.msg.baro.altitude, (int)rx.msg.status.fsw_status,
                (int)rx.msg.status.rocket_state,
                (int)rx.msg.status.airbrake_status);
        }
        else
        {
            printf("[FAIL] count=%lu\r\n", (unsigned long)count);
        }

        count++;

        DelayMs(100);
    }

    return 0;
}
