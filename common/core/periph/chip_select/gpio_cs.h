#pragma once

#include "gpio.h"

namespace LBR
{
class GpioChipSelect
{
public:
    explicit GpioChipSelect(Gpio& cs_pin_);
    void ChipSelectEnable();
    void ChipSelectDisable();

private:
    Gpio& cs_pin;
};
}  // namespace LBR