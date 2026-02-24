/**
 * @file st_gpio.h
 * @brief Gpio driver specfiics for the stmh7.
 * @author Kent Hong
 */

#pragma once

#include <cstdint>
#include "gpio.h"
#include "stm32h723xx.h"

#define ST_GPIO_MAX_PINS 16

namespace LBR
{
namespace Stmh7
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
    explicit HwGpio(const StGpioParams& params_);

    /** 
     * @brief Initializes gpio settings.
     * @return true if success.
     */
    bool init();

    /**
     * @brief toggles pin.
     * @return Returns true if success.
     */
    bool toggle() override;

    /**
     * @brief sets gpio state.
     * @param active bool to set gpio with.
     * @return Returns true if success.
     */
    bool set(const bool active) override;

    /**
     * @brief Reads input register.
     * @return Returns bool of the input register.
     */
    bool read() override;

private:
    StGpioSettings settings;
    const uint8_t pin_num;
    GPIO_TypeDef* const base_addr;
};
}  // namespace Stmh7
}  // namespace LBR