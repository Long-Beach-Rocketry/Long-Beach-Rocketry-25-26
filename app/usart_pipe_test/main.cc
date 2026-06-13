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
        // Fill all three sub-messages to exercise the full protobuf payload
        // Hardcoded values for testing for now, but could be replaced with real data later.
        tx.msg = RocketMessage_init_default;

        tx.msg.has_header = true;
        tx.msg.header.device_id = 0x01;
        tx.msg.header.packet_count = count;
        tx.msg.header.timestamp_ms = count * 10;

        tx.msg.has_imu = true;
        tx.msg.imu.accel_x = 1.0f;
        tx.msg.imu.accel_y = 2.0f;
        tx.msg.imu.accel_z = 9.81f;
        tx.msg.imu.gyro_x = 0.1f;
        tx.msg.imu.gyro_y = 0.2f;
        tx.msg.imu.gyro_z = 0.3f;

        tx.msg.has_baro = true;
        tx.msg.baro.pressure = 101325.0f;
        tx.msg.baro.temperature = 25.0f;

        pipeline.send(&tx, board.usart);

        // Wait for all bytes to arrive back via loopback (~22ms at 115200 baud)
        for (volatile uint32_t i = 0; i < 5000000; i++);

        bool ok = pipeline.receive(&rx, board.usart);

        if (ok)
        {
            printf(
                "[PASS] pkt=%lu dev=0x%lx ts=%lu | ax=%.2f ay=%.2f az=%.2f | "
                "p=%.1f t=%.1f\r\n",
                (unsigned long)rx.msg.header.packet_count,
                (unsigned long)rx.msg.header.device_id,
                (unsigned long)rx.msg.header.timestamp_ms, rx.msg.imu.accel_x,
                rx.msg.imu.accel_y, rx.msg.imu.accel_z, rx.msg.baro.pressure,
                rx.msg.baro.temperature);
        }
        else
        {
            printf("[FAIL] pkt=%lu\r\n", (unsigned long)count);
        }

        count++;

        for (volatile uint32_t i = 0; i < 5000000; i++);
    }

    return 0;
}
