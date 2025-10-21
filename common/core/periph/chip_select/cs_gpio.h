#include "gpio.h"

namespace LBR
{
class GpioCS
{
public:
    explicit GpioCS(Gpio& cs_pin_);
    void ChipSelect.High();
    void ChipSelect.Low();

private:
    Gpio& cs_pin;
};
}  // namespace LBR
