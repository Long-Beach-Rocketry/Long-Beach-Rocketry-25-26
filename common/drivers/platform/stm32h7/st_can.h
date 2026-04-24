/**
* @file st_can.h
* @author Bex Saw
* @brief CAN driver for STM32H7 series microcontrollers.
* @date 4/11/2026
*/

#pragma once 
#include "can.h"

namespace MM
{

Struct StCanParams
{
    CAN_TypeDef* base_addr;
    // Adding CAN specific 
};

class HwCan : public Can
{
public:
    // Constructor 

    // Functions API

private:
    // Private members, such as base address, etc.

};
} // namespace MM