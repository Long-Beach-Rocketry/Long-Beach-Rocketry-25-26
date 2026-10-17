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

void init(Bno055* imu_, Bmp390* baro_);
bool blink_cmd_handler(CmdArgs& args, std::span<uint8_t> txOut);

}  // namespace SensorCmd

}  // namespace LBR