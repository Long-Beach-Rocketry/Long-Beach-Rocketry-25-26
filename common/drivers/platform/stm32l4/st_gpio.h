/**
 * @file st_gpio.h
 * @brief Gpio driver specfiics for the stml4.
 * @author TJ Malaska
 * @date 9/13/2025
 */

#pragma once //ensures file is only used once

#include <cstdint> //for fixed widths like std::uint8_t
#include "gpio.h" //abstract class: in io
#include "stm32l476xx.h" //register definitions for GPIO register access

#define ST_GPIO_MAX_PINS 16 //# of pins per port on STM32

namespace LBR //project name / org
{
namespace Stml4 //scope for STM32l4 specific code
{

enum class GpioMode : uint8_t
{
    INPUT = 0, //input mode: buttons;sensor input
    GPOUT, //general purpose output; LED enable signal (=1)
    ALT_FUNC, //alternate function mode; SPI etc (=2)
    ANALOG, //Analog input,output  (=3)
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