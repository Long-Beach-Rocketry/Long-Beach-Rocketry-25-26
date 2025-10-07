#include "gpio_cs.h"

LBR::GpioChipSelect::GpioChipSelect(Gpio& cs_pin_) : cs_pin{cs_pin_}
{
}