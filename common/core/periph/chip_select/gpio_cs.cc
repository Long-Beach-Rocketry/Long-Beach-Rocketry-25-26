#include "gpio_cs.h"

LBR::GpioChipSelect::GpioChipSelect(LBR::Gpio& cs_pin_) : cs_pin{cs_pin_}
{
}

void LBR::GpioChipSelect::ChipSelectEnable()
{
    cs_pin.set(1);
}

void LBR::GpioChipSelect::ChipSelectDisable()
{
    cs_pin.set(0);
}