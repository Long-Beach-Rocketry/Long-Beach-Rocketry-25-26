#pragma once

#include "ms561101.h"

namespace LBR
{

struct Board
{
    ms561101& barometer;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace LBR