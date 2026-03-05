<<<<<<< HEAD
#include "gpio_cs.h"

LBR::GpioChipSelect::GpioChipSelect(LBR::Gpio& cs_pin_) : cs_pin{cs_pin_}
{
}

void LBR::GpioChipSelect::cs_enable()
{
    cs_pin.set(0);
}

void LBR::GpioChipSelect::cs_disable()
{
    cs_pin.set(1);
}
=======
#include "gpio_cs.h"

LBR::GpioChipSelect::GpioChipSelect(LBR::Gpio& cs_pin_) : cs_pin{cs_pin_}
{
}

void LBR::GpioChipSelect::ChipSelectEnable()
{
    cs_pin.set(0);
}

void LBR::GpioChipSelect::ChipSelectDisable()
{
    cs_pin.set(1);
}
>>>>>>> 319d772 (ekf)
