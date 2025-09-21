#pragma once

namespace LBR {
    class I2C {
        public:
            virtual void Init() = 0;
            virtual bool SetTarget(uint8_t DevAddr) = 0;
            virtual bool Read(uint8_t* data, uint8_t size) = 0;
            virtual bool Write(const uint8_t* data, uint8_t size) = 0;

            ~I2C() = default;
    }
}