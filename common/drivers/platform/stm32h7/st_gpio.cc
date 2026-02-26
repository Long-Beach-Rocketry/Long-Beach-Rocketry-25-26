#include "st_gpio.h"

namespace LBR
{
namespace Stmh7
{

constexpr uint8_t kGpioMaxPinsJK{12};  // Max # of pins for Port J and K
constexpr uint8_t kGpioMaxPinsAH{16};  // Max # of pins for Port A through H

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

bool HwGpio::init()
{
    // Check if port pin num is valid
    if (base_addr == GPIOJ || base_addr == GPIOK)
    {
        if (pin_num >= kGpioMaxPinsJK)
            return false;
    }
    else
    {
        if (pin_num >= kGpioMaxPinsAH || base_addr == nullptr)
            return false;
    }

    uint8_t afr_section = pin_num / 8u;
    uint8_t af_index = (afr_section == 1) ? pin_num - 8 : pin_num;

    // Set fields
    set_field(&base_addr->MODER, static_cast<uint32_t>(settings.mode),
              static_cast<uint32_t>(pin_num), 2);
    set_field(&base_addr->OTYPER, static_cast<uint32_t>(settings.otype),
              static_cast<uint32_t>(pin_num), 1);
    set_field(&base_addr->OSPEEDR, static_cast<uint32_t>(settings.ospeed),
              static_cast<uint32_t>(pin_num), 2);
    set_field(&base_addr->PUPDR, static_cast<uint32_t>(settings.pupd),
              static_cast<uint32_t>(pin_num), 2);
    set_field(&base_addr->AFR[afr_section], static_cast<uint32_t>(settings.af),
              static_cast<uint32_t>(af_index), 4);

    return true;
}

};  // namespace Stmh7
};  // namespace LBR