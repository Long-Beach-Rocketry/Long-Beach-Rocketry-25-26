#include <cstdio>
#include "board.h"
#include "usart_pipe.h"

using namespace LBR;

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

    Pipeline pipeline(board.crc);

    PbCmd tx{};
    PbCmd rx{};
    uint32_t count = 0;

    while (1)
    {
        // Feed fake data
        tx.msg.timestamp_us = count * 1000;

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

        pipeline.send(&tx, board.usart);

        // Wait for all bytes to arrive back via loopback (~22ms at 115200 baud)
        for (volatile uint32_t i = 0; i < 5000000; i++);

        bool ok = pipeline.receive(&rx, board.usart);

        if (ok)
        {
            printf(
                "[PASS] count=%lu ts=%lu | ax=%.2f ay=%.2f az=%.2f | "
                "p=%.1f alt=%.1f | state=%d\r\n",
                (unsigned long)count, (unsigned long)rx.msg.timestamp_us,
                rx.msg.imu.accel_x, rx.msg.imu.accel_y, rx.msg.imu.accel_z,
                rx.msg.baro.air_pressure, rx.msg.baro.altitude,
                (int)rx.msg.status.rocket_state);
        }
        else
        {
            printf("[FAIL] count=%lu\r\n", (unsigned long)count);
        }

        count++;

        for (volatile uint32_t i = 0; i < 5000000; i++);
    }

    return 0;
}
