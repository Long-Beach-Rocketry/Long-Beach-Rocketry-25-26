/**
* @file bno055.h
* @brief BNO055 IMU sensor interface
* @author Bex Saw
* @date 2025-10-22
* @note This file contains the interface for the BNO055 IMU sensor, including initialization, reading, and calibration functions.
*/

#pragma once
#include <cstdint>
#include "imu.h"
#include "st_i2c.h"

namespace LBR {
    class Bno055 : public Imu {
    public:
        explicit Bn055(LBR::Stml4::StI2c& i2c, uint8_t addr = 0x28);
        void Init() override;
        void Update() override;
        void GetAcceleration(float& x, float& y, float& z) override;
        void GetGyroscope(float& x, float& y, float& z) override;
        void GetEulerAngles(float& roll, float& pitch, float& yaw) override;
        void Calibrate() override;
        ~Bno055() override = default;

    private:
        void WriteRegister(uint8_t reg, uint8_t value);
        uint8_t ReadRegister(uint8_t reg);
        void ReadMultipleRegisters(uint8_t startReg, uint8_t* buffer, size_t length);

        LBR::Stml4::StI2c& i2c_;
        uint8_t address_;

        //Common BN055 registers
        static constexpr uint8_t REG_CHIP_ID = 0x00;
        static constexpr uint8_t REG_OPR_MODE = 0x3D;
        static constexpr uint8_t REG_PWR_MODE = 0x3E;
        static constexpr uint8_t REG_PAGE_ID  = 0x07;

        // Data Registers
        static constexpr uint8_t REG_ACC_START = 0x08;
        static constexpr uint8_t REG_GYR_START = 0x14;
        static constexpr uint8_t REG_EUL_START = 0x1A;

        // Mode constants 
        static constexpr uint8_t MODE_CONFIG = 0x00;
        static constexpr uint8_t MODE_NDOF   = 0x0C;

        // Power modes
        static constexpr uint8_t PWR_MODE_NORMAL = 0x00;
    };
}