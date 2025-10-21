#include "cs_gpio.h"

LBR::GpioCS::GpioCS(LBR::Gpio& cs_pin_)
    : cs_pin(cs_pin_)
{
}

void LBR::GpioCS::ChipSelect::High()
{
    cs_pin.set(true);
}
void LBR::GpioCS::ChipSelect::Low()
{
    cs_pin.set(false);
}