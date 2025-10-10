#include "st_gpio.h"
#include "stm32l476xx.h"

#include <cstdint>

namespace LBR
{
namespace Stml4
{
static inline void set_field(volatile uint32_t* field, uint32_t val,
                             uint32_t pos,
                             uint32_t bits)  // size
{
    uint32_t mask{(0x1 << bits) - 1u};
    *field &= ~(mask << (pos * bits));
    *field |= (mask & val) << (pos * bits);
}

HwGpio::HwGpio(const StGpioParams& params)
    : settings_{params.settings},
      pin_num_{params.pin_num},
      base_addr_{params.base_addr} {};

bool HwGpio::init(void)
{
    if (pin_num_ >= ST_GPIO_MAX_PINS || base_addr_ == nullptr)
    {
        return false;
    }
    uint8_t afr_section = pin_num_ / 8u;
    uint8_t af_index = (afr_section == 1) ? pin_num_ - 8 : pin_num_;

    set_field(&base_addr_->MODER, uint32_t(settings_.mode), uint32_t(pin_num_),
              2);
    set_field(&base_addr_->OTYPER, uint32_t(settings_.otype),
              uint32_t(pin_num_), 1);
    set_field(&base_addr_->OSPEEDR, uint32_t(settings_.ospeed),
              uint32_t(pin_num_), 2);
    set_field(&base_addr_->PUPDR, uint32_t(settings_.pupd), uint32_t(pin_num_),
              2);
    set_field(&base_addr_->AFR[afr_section], settings_.af, af_index, 4);

    return true;
}

bool HwGpio::toggle(void)
{
    uint32_t bit = (1u << pin_num_);
    bool current_state = (base_addr_->ODR & bit) != 0u;
    return HwGpio::set(!current_state);
}

bool HwGpio::set(const bool active)
{
    if (active)
    {
        base_addr_->BSRR |= 1u << (pin_num_);
    }
    else
    {
        base_addr_->BSRR |= 1u << (pin_num_ + 16);
    }
    return true;
}

bool HwGpio::read(void)
{
    return base_addr_->IDR & (1u << pin_num_);
}

}  // namespace Stml4
}  // namespace LBR