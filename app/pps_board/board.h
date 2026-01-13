/**
 * @file board.h
 * @brief PPS test board interface
 * @author Bex Saw
 * @date 10/21/2025
 */

#pragma once

// When the ADC driver is written
//#include "uart.h"
//#include "adc.h"
//#include "pwm.h"
//#include encoder driver when written


namespace LBR
{

struct Board
{
    Pwm& pwm;
    //Uart& uart;
    //Adc& adc;
    //Encoder& encoder;
};

bool bsp_init(void);
Board& get_board();
}  // namespace LBR