#include "gpio_cs.h"

LBR::GpioChipSelect::GpioChipSelect(LBR::Gpio& cs_pin_, GPIO_TypeDef* instance_) : cs_pin{cs_pin_}, instance{instance_}
{
}

void GpioChipSelect::ChipSelectEnable() {
    
}