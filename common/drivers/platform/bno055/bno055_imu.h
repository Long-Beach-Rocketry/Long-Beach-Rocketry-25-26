/**
* @file bno055_imu.h
* @brief BNO055 IMU sensor interface
* @author Bex Saw
* @date 2025-10-22
* @note This file contains the interface for the BNO055 IMU sensor, including initialization, reading, and calibration functions.
*/

#pragma once
#include <cstdint>
#include "imu.h"
#include "stm32l4xx.h"
#include "st_i2c.h"

namespace LBR {
    class Bno055 : public Imu {
    public:
        explicit Bno055(LBR::Stml4::HwI2c& i2c_, uint8_t addr = 0x28);

        // Core IMU interface methods
        void Init() override;
        void Update() override;

        // Sensor data retrieval methods
        void GetAcceleration(float& ax, float& ay, float& az) override;
        void GetGyroscope(float& gx, float& gy, float& gz) override;
        void GetEuler(float& roll, float& pitch, float& yaw) override;
        void SensorFusionUpdate() override;

        // Calibration and status methods
        uint8_t Calibrate() override;
        uint8_t GetSystemStatus() override;
        uint8_t GetSystemError() override;


        ~Bno055() override = default;

    private:
        // I2C communication methods or helpers
        void WriteRegister(uint8_t reg, uint8_t value);
        uint8_t ReadRegister(uint8_t reg);
        void ReadMultipleRegisters(uint8_t startReg, uint8_t* buffer, size_t length);
        void SetMode(uint8_t mode);
        

        // I2C interface and device address
        LBR::Stml4::StI2c& i2c_;
        uint8_t address_;
        float euler_[3] = {0.0f, 0.0f, 0.0f}; // Roll, Pitch, Yaw

        //Default I2C address for BNO055
        static constexpr uint8_t BNO055_I2C_ADDR = 0x28;

        //Common BN055 registers
        static constexpr uint8_t REG_CHIP_ID      = 0x00;
        static constexpr uint8_t REG_OPR_MODE     = 0x3D;
        static constexpr uint8_t REG_PWR_MODE     = 0x3E;
        static constexpr uint8_t REG_PAGE_ID      = 0x07;
        static constexpr uint8_t REG_SYS_TRIGGER  = 0x3F;

        // Data Registers
        static constexpr uint8_t REG_ACC_START = 0x08;
        static constexpr uint8_t REG_GYR_START = 0x14;
        static constexpr uint8_t REG_EUL_START = 0x1A;

        // Mode constants 
        static constexpr uint8_t MODE_CONFIG = 0x00;
        static constexpr uint8_t MODE_NDOF   = 0x0C;

        // Power modes
        static constexpr uint8_t PWR_MODE_NORMAL = 0x00;

        // Register addresses for raw data
        // Accelerometer data registers
        static constexpr uint8_t REG_ACCEL_X_LSB = 0x08;
        static constexpr uint8_t REG_ACCEL_X_MSB = 0x09;
        static constexpr uint8_t REG_ACCEL_Y_LSB = 0x0A;
        static constexpr uint8_t REG_ACCEL_Y_MSB = 0x0B;
        static constexpr uint8_t REG_ACCEL_Z_LSB = 0x0C;
        static constexpr uint8_t REG_ACCEL_Z_MSB = 0x0D;
        // Gyroscope data registers
        static constexpr uint8_t REG_GYRO_X_LSB = 0x14;
        static constexpr uint8_t REG_GYRO_X_MSB = 0x15;
        static constexpr uint8_t REG_GYRO_Y_LSB = 0x16;
        static constexpr uint8_t REG_GYRO_Y_MSB = 0x17;
        static constexpr uint8_t REG_GYRO_Z_LSB = 0x18;
        static constexpr uint8_t REG_GYRO_Z_MSB = 0x19;


        // Calibration and status registers 
        static constexpr uint8_t REG_CALIB_STAT = 0x35;
        static constexpr uint8_t REG_SYS_STATUS = 0x39;
        static constexpr uint8_t REG_SYS_ERR    = 0x3A;

        //Euler fused data (not sure if needed)
        static constexpr uint8_t REG_EUL_START = 0x1A;

        float euler_[3] = {0.0f, 0.0f, 0.0f}; // Roll, Pitch, Yaw   

        // Operating mode register and value
        static constexpr uint8_t VAL_OPR_MODE_NDOF = 0x0C;

    };  
}