/**
* @file st_timebase.h
* @brief stm32H723 timebase driver interface
* @author Bex Saw
*/

#include "timebase.h"

namespace LBR
{
namespace Stmh7
{

    // enum definition for all of the timebase specific

class HwTimebase : public Timebase
{
public:
    // explicit constructor 

    // override the function given

private:

    // more variables coming up 
    TIM_TypeDef* base_addr; 
}
}
}