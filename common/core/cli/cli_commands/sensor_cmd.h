/**
 * @file blink_cmd.h
 * @author Joseph Chang
 * @brief The header for the blink command module
 */

#pragma once
#include <span>
#include "cmd.h"

namespace LBR
{

class Bno055;
class Bmp390;

namespace SensorCmd
{
extern const Cmd sensorCmd;

/**
 * @brief Initializes the sensor command with used sensors
 * @param imu_ Pointer to Bno055 object
 * @param baro_ Pointer to Bmp390 object
 */
void init(Bno055* imu_, Bmp390* baro_);

/**
 * @brief Outputs command usage and requested sensor data
 * @param args CmdArgs object with argument count and tokenized run arguments
 * @param txOut Output that we want to share in CLI, could be requested data or errors
 */
bool sensor_cmd_handler(CmdArgs& args, std::span<uint8_t> txOut);

}  // namespace SensorCmd

}  // namespace LBR