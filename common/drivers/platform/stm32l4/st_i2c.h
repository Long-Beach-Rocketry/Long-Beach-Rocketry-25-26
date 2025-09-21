#pragma once

#include "stm32l4xx.h"
#include "st_gpio.h"
#include <stdint.h>
#include <stdbool.h>

namespace LBR {
    namespace Stml4 {
        class I2C {

            public:
                I2C(I2C_TypeDef* instance);
                void Init();
                bool SetTarget(uint8_t DevAddr);
                bool Read(uint8_t* data, uint8_t size);
                bool Write(const uint8_t* data, uint8_t size);

            private:
                I2C_TypeDef* _instance;              //holds which I2C peripheral we're using
                uint32_t _timing;
                HwGpio _gpio1;
                HwGpio _gpio2;
                
        }
    }
}