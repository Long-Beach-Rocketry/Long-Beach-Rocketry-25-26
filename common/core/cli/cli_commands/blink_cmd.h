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

class Gpio;

namespace BlinkCmd
{
extern const Cmd blinkCmd;

void init(Gpio* led1, Gpio* led2, Gpio* led3);
bool blink_cmd_handler(CmdArgs& args, std::span<uint8_t> txOut);

}  // namespace BlinkCmd

}  // namespace LBR