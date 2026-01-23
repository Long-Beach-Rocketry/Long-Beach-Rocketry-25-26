#pragma once

#include "gpio.h"

namespace LBR
{
class GpioChipSelect
{
public:
    explicit GpioChipSelect(Gpio& cs_pin_);
    void cs_enable();
    void cs_disable();

private:
    Gpio& cs_pin;
};
}  // namespace LBR