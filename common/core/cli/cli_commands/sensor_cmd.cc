#include "sensor_cmd.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <format>
#include "bmp390.h"
#include "bno055_imu.h"
#include "string_span.h"

namespace LBR
{
// TODO: replace with sensor manager
static Bno055* imu{nullptr};
static Bmp390* baro{nullptr};

namespace SensorCmd
{

void init(Bno055* imu_, Bmp390* baro_)
{
    imu = imu_;
    baro = baro_;
}

bool sensor_cmd_handler(const CmdArgs& args, std::span<uint8_t> txOut)
{
    std::span<const uint8_t> error = sv_to_span(kCmdErrorArgs);
    std::copy(error.begin(), error.end(),
              txOut.subspan(0, error.size()).begin());

    std::span<const uint8_t> success = sv_to_span(kCmdSuccess);

    if (args.argc > 0)
    {
        std::string_view op1(args.argv[0]);
        if (op1 == "--help" || op1 == "-h")
        {
            std::span<const uint8_t> desc = sv_to_span(sensorCmd.desc);
            std::copy(desc.begin(), desc.end(),
                      txOut.subspan(0, desc.size()).begin());
            return true;
        }
        else if (op1 == "-i" || op1 == "--imu")
        {
            if (!imu)
            {
                std::format_to_n(txOut.data(), txOut.size() - 1,
                                 "ERROR: IMU not found!\r\n\n");
                return false;
            }
            Bno055Data imu_data{};
            if (imu->read_all(imu_data))
            {
                if (args.argc == 1)
                {
                    std::format_to_n(
                        txOut.data(), txOut.size() - 1,
                        "Imu Data\r\n"
                        "\tAcceleration (x,y,z):\t{},{},{}\r\n"
                        "\tGyroscope    (x,y,z):\t{},{},{}\r\n"
                        "\tLinear Accel (x,y,z):\t{},{},{}\r\n"
                        "\tGravity      (x,y,z):\t{},{},{}\r\n"
                        "\tQuaternion (w,x,y,z):\t{},{},{},{}\r\n\n",
                        imu_data.accel.x, imu_data.accel.y, imu_data.accel.z,
                        imu_data.gyro.x, imu_data.gyro.y, imu_data.gyro.z,
                        imu_data.linear_accel.x, imu_data.linear_accel.y,
                        imu_data.linear_accel.z, imu_data.gravity.x,
                        imu_data.gravity.y, imu_data.gravity.z, imu_data.quat.w,
                        imu_data.quat.x, imu_data.quat.y, imu_data.quat.z);
                    return true;
                }
                else
                {
                    std::string_view op2(args.argv[1]);
                    if (op2 == "a" || op2 == "acc")
                    {
                        std::format_to_n(
                            txOut.data(), txOut.size() - 1,
                            "Imu Data\r\n"
                            "\tAcceleration (x,y,z):\t{},{},{}\r\n"
                            "\tGyroscope    (x,y,z):\t{},{},{}\r\n"
                            "\tLinear Accel (x,y,z):\t{},{},{}\r\n"
                            "\tGravity      (x,y,z):\t{},{},{}\r\n"
                            "\tQuaternion (w,x,y,z):\t{},{},{},{}\r\n\n",
                            imu_data.accel.x, imu_data.accel.y,
                            imu_data.accel.z, imu_data.gyro.x, imu_data.gyro.y,
                            imu_data.gyro.z, imu_data.linear_accel.x,
                            imu_data.linear_accel.y, imu_data.linear_accel.z,
                            imu_data.gravity.x, imu_data.gravity.y,
                            imu_data.gravity.z, imu_data.quat.w,
                            imu_data.quat.x, imu_data.quat.y, imu_data.quat.z);
                    }
                    else if (op2 == "a" || op2 == "acc")
                    {
                        std::format_to_n(
                            txOut.data(), txOut.size() - 1,
                            "Imu Data\r\n"
                            "\tAcceleration (x,y,z):\t{},{},{}\r\n",
                            imu_data.accel.x, imu_data.accel.y,
                            imu_data.accel.z);
                    }
                    else if (op2 == "g" || op2 == "grav")
                    {
                        std::format_to_n(
                            txOut.data(), txOut.size() - 1,
                            "Imu Data\r\n"
                            "\tGravity      (x,y,z):\t{},{},{}\r\n",
                            imu_data.gravity.x, imu_data.gravity.y,
                            imu_data.gravity.z);
                    }
                    else if (op2 == "y" || op2 == "gyro")
                    {
                        std::format_to_n(
                            txOut.data(), txOut.size() - 1,
                            "Imu Data\r\n"
                            "\tGyroscope    (x,y,z):\t{},{},{}\r\n",
                            imu_data.gyro.x, imu_data.gyro.y, imu_data.gyro.z);
                    }
                    else if (op2 == "l" || op2 == "lacc")
                    {
                        std::format_to_n(
                            txOut.data(), txOut.size() - 1,
                            "Imu Data\r\n"
                            "\tLinear Accel (x,y,z):\t{},{},{}\r\n",
                            imu_data.linear_accel.x, imu_data.linear_accel.y,
                            imu_data.linear_accel.z);
                    }
                    else if (op2 == "q" || op2 == "quat")
                    {
                        std::format_to_n(
                            txOut.data(), txOut.size() - 1,
                            "Imu Data\r\n"
                            "\tQuaternion (w,x,y,z):\t{},{},{},{}\r\n\n",
                            imu_data.quat.w, imu_data.quat.x, imu_data.quat.y,
                            imu_data.quat.z);
                    }
                    else
                    {
                        std::format_to_n(txOut.data(), txOut.size() - 1,
                                         "ERROR: Invalid argument\r\n\n");
                        return false;
                    }
                    return true;
                }
            }
            else
            {
                std::format_to_n(txOut.data(), txOut.size() - 1,
                                 "ERROR: Could not read IMU data!\r\n\n");

                return false;
            }
        }
        else if (op1 == "-b" || op1 == "--baro")
        {
            if (!baro)
            {
                std::format_to_n(txOut.data(), txOut.size() - 1,
                                 "ERROR: Barometer not found!\r\n\n");
                return false;
            }
            float pressure = baro->get_pressure();
            float temperature = baro->get_temperature();
            if (args.argc == 1)
            {
                std::format_to_n(txOut.data(), txOut.size() - 1,
                                 "Barometer Data\r\n"
                                 "\tTemperature:\t {} C\r\n"
                                 "\tPressure   :\t {} Pa\r\n\n",
                                 temperature, pressure);
                return true;
            }
            else
            {
                std::string_view op2(args.argv[1]);
                if (op2 == "p" || op2 == "press")
                {
                    std::format_to_n(txOut.data(), txOut.size() - 1,
                                     "Barometer Data\r\n"
                                     "\tPressure   :\t {} Pa\r\n\n",
                                     pressure);
                }
                else if (op2 == "t" || op2 == "temp")
                {
                    std::format_to_n(txOut.data(), txOut.size() - 1,
                                     "Barometer Data\r\n"
                                     "\tTemperature:\t {} C\r\n",
                                     temperature);
                }
                else
                {
                    std::format_to_n(txOut.data(), txOut.size() - 1,
                                     "ERROR: Invalid argument\r\n\n");
                    return false;
                }
                return true;
            }
        }
        std::format_to_n(txOut.data(), txOut.size() - 1,
                         "ERROR: Invalid argument\r\n\n");

        return false;
    }
    std::format_to_n(txOut.data(), txOut.size() - 1,
                     "ERROR: No arguments specified\r\n\n");

    return false;
}

const Cmd sensorCmd = {
    .name = "sensor",
    .desc =
        "Usage: sensor [sensor]\r\n\n"
        "Reads data from the sensor manager.\r\n\n"
        "Options:\r\n\t"
        "-i, --imu [a, acce | g, gyro | l, lacc | g, grav | q, quat]\r\n\t"
        "Outputs specified imu data\r\n\n\t"
        "-b, --baro [p, press | t, temp]\r\n\t"
        "Outputs the specified barometer data\r\n\n"
        "\t-h, --help\t\tShows this message.\r\n",
    .invoke = sensor_cmd_handler};

}  // namespace SensorCmd

}  // namespace LBR