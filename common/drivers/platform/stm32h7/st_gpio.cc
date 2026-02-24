#include "st_gpio.h"

namespace LBR
{
namespace Stmh7
{
static inline void set_field(volatile uint32_t* reg, uint32_t val, uint32_t pos,
                             uint32_t bits)
{
    uint32_t mask{(1u << bits) - 1u};
    *reg &= ~(mask << (pos * bits));
    *reg |= (val & mask) << (pos * bits);
}

HwGpio::HwGpio(const StGpioParams& params_)
    : settings{params_.settings},
      pin_num{params_.pin_num},
      base_addr{params_.base_addr}
{
}
};  // namespace Stmh7
};  // namespace LBR