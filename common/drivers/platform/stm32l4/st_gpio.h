/**
 *
 *
 */

#pragma once

#include "stm32l476xx.h"

#include "gpio.h"

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
            LOW = 0,
            MEDIUM,
            HIGH,
            VERY_HIGH
        };

        typedef struct // check later if this should be constexpr? also how to delcare structs cpp
        {
            GpioMode mode;
            GpioOtype otype;
            GpioOspeed ospeed;
            GpioPupd pupd;
            // uint8_t af;
        } StGpioSettings; // controll registers

        typedef struct
        {
            StPrivGpio priv;
            uint32_t base_addr;
            uint8_t pin_num;
            StGpioSettings conf;
        } StGpioParams;

        class HwGpio : public Gpio
        {
        public:
            HwGpio(StGpioSettings settings, uint8_t pin_num, GPIO_TypeDef base_addr);
            bool init(void) override;
            bool toggle(void) override;
            bool set(const bool active) override;
            bool read(void) override;

        private:
            uint8_t pin_num; // should this be const?
            GPIO_TypeDef base_addr;
            StGpioSettings settings;
        };
    }
}