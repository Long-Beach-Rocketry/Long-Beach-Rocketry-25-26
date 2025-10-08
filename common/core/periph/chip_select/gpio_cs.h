#pragma once

#include "gpio.h"

namespace LBR
{
class GpioChipSelect
{
public:
    explicit GpioChipSelect(Gpio& cs_pin_, GPIO_TypeDef* instance_);
    void ChipSelectEnable();
    void ChipSelectDisable();

private:
    Gpio& cs_pin;
    GPIO_TypeDef* instance;
};
}  // namespace LBR