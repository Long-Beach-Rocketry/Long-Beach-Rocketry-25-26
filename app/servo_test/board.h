/**
 * @file board.h
 * @brief BSP interface for servo motor testing
 * @author Yshi Blanco
 * @date 01/28/2026
 */

#pragma once
#include "servo_motor.h"

namespace LBR
{

struct Board
{
    ServoMotor& servo_motor;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace LBR