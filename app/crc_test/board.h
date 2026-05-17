/**
 * @file board.h
 * @brief CRC driver interface
 * @author Joseph Chang
 */

#pragma once
#include "crc.h"

namespace LBR
{

struct Board
{
    Crc& crc;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace LBR