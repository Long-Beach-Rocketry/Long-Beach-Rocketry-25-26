#include "st_gpio.h"
#include "stm32l476xx.h"

#include <cstdint>

namespace LBR
{
namespace Stml4
{
static inline void set_field(volatile std::uint32_t* field, std::uint32_t val,
                             std::uint32_t pos,
                             std::uint32_t bits)  // size
{
    std::uint32_t mask{(0x1 << bits) - 1u};
    *field &= ~(mask << (pos * bits));
    *field |= (mask & val) << (pos * bits);
}

//not using currently
static inline void enable_gpio_clock(GPIO_TypeDef* gpio)
{
    if (gpio == GPIOA)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    else if (gpio == GPIOB)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    else if (gpio == GPIOC)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    else if (gpio == GPIOD)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
    else if (gpio == GPIOE)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
    else if (gpio == GPIOF)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOFEN;
    else if (gpio == GPIOG)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;
    else if (gpio == GPIOH)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOHEN;
}

HwGpio(StGpioSettings& settings, std::uint8_t pin_num, GPIO_TypeDef* base_addr)
    : settings_(settings), pin_num_(pin_num), base_addr_(base_addr){};

// enable clock in bsp
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

// not needed
// bool HwGpio::toggle(void)
// {
// }

bool HwGpio::set(const bool active)
{
    if (active)
    {
        base_addr_->BSRR |= 1u << (pin_num_ + 16);
    }
    else
    {
        base_addr_->BSRR |= 1u << (pin_num_);
    }
}

bool HwGpio::read(void)
{
    return base_addr_->IDR & (1u << pin_num_);
}

}  // namespace Stml4
}  // namespace LBR