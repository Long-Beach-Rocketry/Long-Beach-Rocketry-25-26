/**
 * @file st_gpio.h
 * @brief Gpio driver specfiics for the stml4.
 * @author TJ Malaska
 * @date 9/13/2025
 */

#pragma once

#include <cstdint>
#include "gpio.h"
#include "stm32l476xx.h"

#define ST_GPIO_MAX_PINS 16

namespace LBR
{
namespace Stml4
{

enum class GpioMode : uint8_t
{
    INPUT = 0,
    GPOUT,
    ALT_FUNC,
    ANALOG,
};

enum class GpioOtype : uint8_t
{
    PUSH_PULL = 0,
    OPEN_DRAIN
};

enum class GpioOspeed : uint8_t
{
    LOW = 0,
    MEDIUM,
    HIGH,
    VERY_HIGH
};

enum class GpioPupd : uint8_t
{
    NO_PULL = 0,
    PULL_UP,
    PULL_DOWN
};

/**
 * @brief Collection of control params to configure gpio.
 */
struct StGpioSettings
{
    GpioMode mode;
    GpioOtype otype;
    GpioOspeed ospeed;
    GpioPupd pupd;
    uint8_t af;
};

/**
 * @brief Collection of settings pin number and base address.
 */
struct StGpioParams
{
    StGpioSettings settings;
    uint8_t pin_num;
    GPIO_TypeDef* base_addr;
};

class HwGpio : public Gpio
{
public:
    /**
     * @brief Hw Contructor
     * @param params struct of port, number, and settings.
     */
    explicit HwGpio(const StGpioParams& params);

    /** 
     * @brief Initializes gpio settings.
     * @return true if success.
     */
    bool init(void);

    bool toggle(void) override;
    bool set(const bool active) override;
    bool read(void) override;

private:
    StGpioSettings settings_;
    const uint8_t pin_num_;
    GPIO_TypeDef* const base_addr_;
};
}  // namespace Stml4
}  // namespace LBR