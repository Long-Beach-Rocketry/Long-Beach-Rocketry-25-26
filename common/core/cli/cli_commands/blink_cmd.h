#pragma once
#include "cmd.h"

namespace LBR
{

class Gpio;

namespace BlinkCmd
{

void init(Gpio* led);

}

}  // namespace LBR