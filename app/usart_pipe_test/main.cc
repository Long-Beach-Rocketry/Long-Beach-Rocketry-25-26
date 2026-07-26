#include <cstdio>
#include "board.h"
#include "delay.h"
#include "usart_pipe.h"

using namespace LBR;
using namespace LBR::Utils;

namespace
{
// Loopback corrupts a value in flight (bad CRC, bit flip) far more usefully
// as a mismatched field than as a silent decode success, so give TX/RX float
// comparisons a small tolerance instead of doing an exact bitwise compare.
bool nearly_equal(float a, float b, float epsilon = 1e-4f)
{
    float diff = a - b;
    if (diff < 0)
        diff = -diff;
    return diff <= epsilon;
}
}  // namespace

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
    uint32_t pass_count = 0;
    uint32_t fail_count = 0;

    while (1)
    {
        // Feed fake data
        //
        // NOTE: fields are assigned one at a time here because this test just
        // fakes numbers for a loopback check. Once real drivers (IMU, baro,
        // and whatever the airbrake app adds later) feed this struct, the
        // per-field spread-out is the wrong shape -- every new sensor means
        // editing the loop again, and status/rocket_state/airbrake_status
        // will need to change dynamically off a real state machine, not sit
        // as constants. That doesn't belong inline in main() either way.
        //
        // Plan: wrap this in a class (working name: Packet) that owns a
        // RocketMessage and exposes a sample() (or similar) that pulls from
        // Board's drivers field-by-sub-message, so main() is just:
        //   packet.sample(board);
        //   board.pipeline.send(&packet.msg, board.usart);
        // Pipeline::send() itself doesn't change -- see
        // common/core/protobuff/docs/sensor_message_guide.md for the full
        // writeup on why RocketMessage is already the "sensor data struct"
        // and how new sensors should be added to it.
        tx.msg.has_status = true;
        tx.msg.status.fsw_status = FswStatus_FSW_STATUS_ACTIVE;
        tx.msg.status.rocket_state = RocketState_ROCKET_STATE_POWERED_FLIGHT;
        tx.msg.status.airbrake_status =
            AirbrakeStatus_AIRBRAKE_STATUS_PRELAUNCH;

        tx.msg.has_imu = true;
        tx.msg.imu.accel_x = 1.0f + (count * 0.1f);
        tx.msg.imu.accel_y = 2.0f;
        tx.msg.imu.accel_z = 9.8f;
        tx.msg.imu.gyro_x = 0.1f;
        tx.msg.imu.gyro_y = 0.2f;
        tx.msg.imu.gyro_z = 0.3f;
        tx.msg.imu.quat_w = 1.0f;
        tx.msg.imu.quat_x = 0.0f;
        tx.msg.imu.quat_y = 0.0f;
        tx.msg.imu.quat_z = 0.0f;
        tx.msg.imu.linear_accel_x = 0.0f;
        tx.msg.imu.linear_accel_y = 0.0f;
        tx.msg.imu.linear_accel_z = 0.0f;
        tx.msg.imu.gravity_x = 0.0f;
        tx.msg.imu.gravity_y = 0.0f;
        tx.msg.imu.gravity_z = 9.8f;

        tx.msg.has_baro = true;
        tx.msg.baro.air_pressure = 101325.0f - (count * 10.0f);
        tx.msg.baro.altitude = 10.0f + (count * 1.5f);
        tx.msg.baro.temperature = 20.0f + (count * 0.05f);

        // Stamp real hardware time right at send
        tx.msg.timestamp_us = GetUs();
        bool sent = board.pipeline.send(&tx, board.usart);

        // Wait for all bytes to arrive back via loopback (~22ms at 115200 baud)
        DelayMs(50);

        bool decoded = board.pipeline.receive(&rx, board.usart);

        // Only count it as a pass if every field actually survived the round
        // trip intact -- decode succeeding just means CRC/EOF matched, not
        // that the payload we get back is the payload we sent.
        bool fields_match =
            nearly_equal(rx.msg.imu.accel_x, tx.msg.imu.accel_x) &&
            nearly_equal(rx.msg.imu.accel_y, tx.msg.imu.accel_y) &&
            nearly_equal(rx.msg.imu.accel_z, tx.msg.imu.accel_z) &&
            nearly_equal(rx.msg.imu.gyro_x, tx.msg.imu.gyro_x) &&
            nearly_equal(rx.msg.imu.gyro_y, tx.msg.imu.gyro_y) &&
            nearly_equal(rx.msg.imu.gyro_z, tx.msg.imu.gyro_z) &&
            nearly_equal(rx.msg.imu.quat_w, tx.msg.imu.quat_w) &&
            nearly_equal(rx.msg.imu.quat_x, tx.msg.imu.quat_x) &&
            nearly_equal(rx.msg.imu.quat_y, tx.msg.imu.quat_y) &&
            nearly_equal(rx.msg.imu.quat_z, tx.msg.imu.quat_z) &&
            nearly_equal(rx.msg.imu.linear_accel_x,
                         tx.msg.imu.linear_accel_x) &&
            nearly_equal(rx.msg.imu.linear_accel_y,
                         tx.msg.imu.linear_accel_y) &&
            nearly_equal(rx.msg.imu.linear_accel_z,
                         tx.msg.imu.linear_accel_z) &&
            nearly_equal(rx.msg.imu.gravity_x, tx.msg.imu.gravity_x) &&
            nearly_equal(rx.msg.imu.gravity_y, tx.msg.imu.gravity_y) &&
            nearly_equal(rx.msg.imu.gravity_z, tx.msg.imu.gravity_z) &&
            nearly_equal(rx.msg.baro.air_pressure, tx.msg.baro.air_pressure) &&
            nearly_equal(rx.msg.baro.altitude, tx.msg.baro.altitude) &&
            nearly_equal(rx.msg.baro.temperature, tx.msg.baro.temperature) &&
            rx.msg.status.fsw_status == tx.msg.status.fsw_status &&
            rx.msg.status.rocket_state == tx.msg.status.rocket_state &&
            rx.msg.status.airbrake_status == tx.msg.status.airbrake_status &&
            rx.msg.timestamp_us == tx.msg.timestamp_us;

        bool ok = sent && decoded && fields_match;

        if (ok)
        {
            pass_count++;
            printf(
                "[PASS] count=%lu ts=%luus tx_len=%u rx_len=%u\r\n"
                "  IMU   accel=[%.2f, %.2f, %.2f]  gyro=[%.3f, %.3f, %.3f]\r\n"
                "        quat=[%.3f, %.3f, %.3f, %.3f]  lin=[%.3f, %.3f, %.3f] "
                " grav=[%.3f, %.3f, %.3f]\r\n"
                "  BARO  press=%.1fPa  alt=%.2fm  temp=%.2fC\r\n"
                "  STAT  fsw=%d  state=%d  ab=%d\r\n",
                (unsigned long)count, (unsigned long)rx.msg.timestamp_us,
                board.pipeline.get_tx_frame_len(),
                board.pipeline.get_rx_frame_len(), rx.msg.imu.accel_x,
                rx.msg.imu.accel_y, rx.msg.imu.accel_z, rx.msg.imu.gyro_x,
                rx.msg.imu.gyro_y, rx.msg.imu.gyro_z, rx.msg.imu.quat_w,
                rx.msg.imu.quat_x, rx.msg.imu.quat_y, rx.msg.imu.quat_z,
                rx.msg.imu.linear_accel_x, rx.msg.imu.linear_accel_y,
                rx.msg.imu.linear_accel_z, rx.msg.imu.gravity_x,
                rx.msg.imu.gravity_y, rx.msg.imu.gravity_z,
                rx.msg.baro.air_pressure, rx.msg.baro.altitude,
                rx.msg.baro.temperature, (int)rx.msg.status.fsw_status,
                (int)rx.msg.status.rocket_state,
                (int)rx.msg.status.airbrake_status);
        }
        else
        {
            fail_count++;
            printf("[FAIL] count=%lu sent=%d decoded=%d fields_match=%d\r\n",
                   (unsigned long)count, sent, decoded, fields_match);
        }

        if ((count % 20) == 0)
        {
            printf("[STATS] pass=%lu fail=%lu\r\n", (unsigned long)pass_count,
                   (unsigned long)fail_count);
        }

        count++;

        DelayMs(100);
    }

    return 0;
}
