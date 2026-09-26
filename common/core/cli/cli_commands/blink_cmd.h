#pragma once
#include "cmd.h"

namespace LBR
{

class Gpio;

namespace BlinkCmd
{
extern const Cmd blinkCmd;

void init(Gpio* led1, Gpio* led2, Gpio* led3);
bool blink_cmd_handler(CmdArgs& args);

}  // namespace BlinkCmd

}  // namespace LBR