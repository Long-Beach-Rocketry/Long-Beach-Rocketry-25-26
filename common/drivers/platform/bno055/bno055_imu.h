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
        void GetFusedEuler(float& roll, float& pitch, float& yaw) override;
        void GetLinearAcceleration(float& lax, float& lay, float& laz) override; //Need?
        void GetGravityVector(float& gx, float& gy, float& gz) override; //Need?
        void SensorFusionUpdate() override;

        // Calibration and status methods
        uint8_t Calibrate() override;
        uint8_t GetSystemStatus() override;
        uint8_t GetSystemError() override;


        ~Bno055() override = default;

    /* For sensor mode configuration */
     enum class MODE : uint8_t {
        CONFIG        = 0x00,
        IMU           = 0x08,
        NDOF          = 0x0C
    };

    /* For power mode configuration */
    enum class PowerMode : uint8_t {
        NORMAL    = 0x00,
        LOW_POWER = 0x01,
        SUSPEND   = 0x02
    };

    // Have all this now (Need to ask and remove some stuff later)
    /* For accelerometer configuration */
    enum class AccelRange : uint8_t {
        RANGE_2G  = 0b00,
        RANGE_4G  = 0b01,
        RANGE_8G  = 0b10,
        RANGE_16G = 0b11
    };

    enum class AccelBandwidth : uint8_t {
        BW_7_81HZ   = 0b000,
        BW_15_63HZ  = 0b001,
        BW_31_25HZ  = 0b010,
        BW_62_5HZ   = 0b011,
        BW_125HZ    = 0b100,
        BW_250HZ    = 0b101,
        BW_500HZ    = 0b110,
        BW_1000HZ   = 0b111
    };

    enum class AccelOpMode : uint8_t {
        NORMAL       = 0b000,
        SUSPEND      = 0b001,
        LOW_POWER_1  = 0b010,
        STANDBY      = 0b011,
        LOW_POWER_2  = 0b100,
        DEEPSUSPEND  = 0b101
    };

    /* For gyroscope configuration */
    enum class GyroRange : uint8_t {
        DPS_2000 = 0b000,
        DPS_1000 = 0b001,
        DPS_500  = 0b010,
        DPS_250  = 0b011,
        DPS_125  = 0b100
    };

    enum class GyroBandwidth : uint8_t {
        BW_523HZ = 0b000,
        BW_230HZ = 0b001,
        BW_116HZ = 0b010,
        BW_47HZ  = 0b011,
        BW_23HZ  = 0b100,
        BW_12HZ  = 0b101,
        BW_64HZ  = 0b110,
        BW_32HZ  = 0b111
    };



    void ConfigureAccelerometer(AccelRange range, AccelBandwidth bandwidth, AccelOpMode opMode);
    void ConfigureGyroscope(GyroRange range, GyroBandwidth bandwidth);
    
    uint8_t RunPowerOnSelfTest();
    uint8_t RunBuiltInSelfTest();

    private:
        // I2C communication methods or helpers
        void WriteRegister(uint8_t reg, uint8_t value);
        uint8_t ReadRegister(uint8_t reg);
        void ReadMultipleRegisters(uint8_t startReg, uint8_t* buffer, size_t length);
        void SetMode(MODE mode);
        

        // I2C interface and device address
        LBR::Stml4::StI2c& i2c_;
        uint8_t address_;
        uint8_t accelData_[6] = {0}; // X, Y, Z accelerometer data
        uint8_t gyroData_[6] = {0};  // X, Y, Z gyroscope data
        float euler_[3] = {0.0f, 0.0f, 0.0f}; // Roll, Pitch, Yaw

        //Default I2C address for BNO055
        static constexpr uint8_t BNO055_I2C_ADDR = 0x28;

        //Common BN055 registers
        static constexpr uint8_t REG_CHIP_ID      = 0x00;
        static constexpr uint8_t REG_OPR_MODE     = 0x3D;
        static constexpr uint8_t REG_PWR_MODE     = 0x3E;
        static constexpr uint8_t REG_PAGE_ID      = 0x07;
        static constexpr uint8_t REG_SYS_TRIGGER  = 0x3F;
        static constexpr uint8_t REG_ST_RESULT   = 0x36; // POST/BIST result bits
        
        // Data Registers
        static constexpr uint8_t REG_ACC_START = 0x08;
        static constexpr uint8_t REG_GYR_START = 0x14;
        static constexpr uint8_t REG_EUL_START = 0x1A;

        // Mode constants 
        static constexpr uint8_t MODE_CONFIG = 0x00;
        static constexpr uint8_t MODE_NDOF   = 0x0C;

        // Power modes
        static constexpr uint8_t PWR_MODE_NORMAL = 0x00;

        /**
        * @brief Data registers for accelerometer and gyroscope readings
        * These registers hold the raw data for acceleration and angular velocity.
        */
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
        
        /**
        * @brief Offset registers for accelerometer and gyroscope calibration
        * These registers are used to store calibration offsets for the accelerometer and gyroscope.
        */
        // Offset Accelerometer registers
        static constexpr uint8_t ACC_OFFSET_X_LSB = 0x55;
        static constexpr uint8_t ACC_OFFSET_X_MSB = 0x56;
        static constexpr uint8_t ACC_OFFSET_Y_LSB = 0x57;
        static constexpr uint8_t ACC_OFFSET_Y_MSB = 0x58;
        static constexpr uint8_t ACC_OFFSET_Z_LSB = 0x59;
        static constexpr uint8_t ACC_OFFSET_Z_MSB = 0x5A;

        // Offset Gyroscope registers
        static constexpr uint8_t GYR_OFFSET_X_LSB = 0x61;
        static constexpr uint8_t GYR_OFFSET_X_MSB = 0x62;
        static constexpr uint8_t GYR_OFFSET_Y_LSB = 0x63;
        static constexpr uint8_t GYR_OFFSET_Y_MSB = 0x64;
        static constexpr uint8_t GYR_OFFSET_Z_LSB = 0x65;
        static constexpr uint8_t GYR_OFFSET_Z_MSB = 0x66;


        // Axis remap registers (if needed)
        static constexpr uint8_t AXIS_REMAP_CONFIG = 0x24;

        // Calibration and status registers 
        static constexpr uint8_t REG_CALIB_STAT = 0x35;
        static constexpr uint8_t REG_SYS_STATUS = 0x39;
        static constexpr uint8_t REG_SYS_ERR    = 0x3A;

        // Operating mode register and value
        static constexpr uint8_t VAL_OPR_MODE_NDOF = 0x0C;

        // Linear acceleration and gravity vector registers
        static constexpr uint8_t REG_LIA_ACCEL_X_LSB = 0x28;
        static constexpr uint8_t REG_LIA_ACCEL_X_MSB = 0x29;
        static constexpr uint8_t REG_LIA_ACCEL_Y_LSB = 0x2A;
        static constexpr uint8_t REG_LIA_ACCEL_Y_MSB = 0x2B;
        static constexpr uint8_t REG_LIA_ACCEL_Z_LSB = 0x2C;
        static constexpr uint8_t REG_LIA_ACCEL_Z_MSB = 0x2D;

        static constexpr uint8_t REG_GRAVITY_X_LSB = 0x2E;
        static constexpr uint8_t REG_GRAVITY_X_MSB = 0x2F;
        static constexpr uint8_t REG_GRAVITY_Y_LSB = 0x30;
        static constexpr uint8_t REG_GRAVITY_Y_MSB = 0x31;
        static constexpr uint8_t REG_GRAVITY_Z_LSB = 0x32;
        static constexpr uint8_t REG_GRAVITY_Z_MSB = 0x33;

    };
}