#pragma once 

#include "pps_board/board.h"

namespace LBR
{
int main(int argc, char* argv[])
{
    bsp_init();
    Board& board = get_board();

    return 0;
}
}  // namespace LBR