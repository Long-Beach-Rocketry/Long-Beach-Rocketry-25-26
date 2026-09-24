#pragma once
#include "cmd.h"

namespace LBR
{

class Gpio;

namespace BlinkCmd
{

void init(Gpio* led);
bool blink_cmd_handler(CmdArgs& args);

}  // namespace BlinkCmd

}  // namespace LBR