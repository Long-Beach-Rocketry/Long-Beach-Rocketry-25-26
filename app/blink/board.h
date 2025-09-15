#pragma once
#include "gpio.h"
//do the cmake

namespace LBR
{
namespace Board
{
bool init();
Gpio& get_led();
}  // namespace Board
}  // namespace LBR