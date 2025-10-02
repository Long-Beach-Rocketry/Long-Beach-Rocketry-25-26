/**
 * @file st_gpio.h
 * @brief Gpio driver specfiics for the stml4.
 * @author TJ Malaska
 * @date 9/13/2025
 *
 * Detailed description of what this file provides.
 * Can span multiple lines if necessary.
 */

#pragma once

#include "gpio.h"
#include "stm32l476xx.h"
#include <cstdint>


#define ST_GPIO_MAX_PINS 16

namespace LBR {
namespace Stml4 {

enum class GpioMode : std::uint8_t {
  INPUT = 0,
  GPOUT,
  ALT_FUNC,
  ANALOG,
};

enum class GpioOtype : std::uint8_t { PUSH_PULL = 0, OPEN_DRAIN };

enum class GpioOspeed : std::uint8_t { LOW = 0, MEDIUM, HIGH, VERY_HIGH };

enum class GpioPupd : std::uint8_t { NO_PULL = 0, PULL_UP, PULL_DOWN };

struct StGpioSettings // controll registers
{
  GpioMode mode;
  GpioOtype otype;
  GpioOspeed ospeed;
  GpioPupd pupd;
  uint8_t af;
};

// struct StGpioParams
// {
//     //StPrivGpio priv;
//     uint32_t base_addr;
//     uint8_t pin_num;
//     StGpioSettings conf;
// };

class HwGpio : public Gpio {
public:
  explicit HwGpio(StGpioSettings &settings,
                  std::uint8_t pin_num, // check if this need to be explicit
                  GPIO_TypeDef *base_addr);
  bool init(void) override;
  // bool toggle(void) override;
  bool set(const bool active) override;
  bool read(void) override;

private:
  StGpioSettings settings_;
  const std::uint8_t pin_num_;
  GPIO_TypeDef *const base_addr_;
};
} // namespace Stml4
} // namespace LBR