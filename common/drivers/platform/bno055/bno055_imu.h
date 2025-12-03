/**
* @file bno055_imu.h
* @brief BNO055 IMU sensor interface
* @author Bex Saw
* @date 2025-10-22
* @note This file contains the interface for the BNO055 IMU sensor, including initialization, reading, and calibration functions.
*/

#pragma once

#include <cstdint>
#include <cstddef>

namespace LBR::Stml4 {
    class HwI2c;
}

namespace LBR {

        // Function pointer types for I2C operations
        using I2cMemReadFn = bool(*)(uint8_t dev_addr, uint8_t reg, uint8_t* data, size_t len);
        using I2cMemWriteFn = bool(*)(uint8_t dev_addr, uint8_t reg, const uint8_t* data, size_t len);

        /**
        * @brief 3D Vector Type for Acceleration and Gyro.
         */

         struct Vec3 {
            float x, y, z;
         };

         /**
         * @brief Quaternion structure reprsenting orientation (** No gimbal lock).
          */

          struct Quaternion {
            float w, x, y, z;
          };

          struct Bno055Data {
            Vec3 accel;
            Vec3 gyro;
            Vec3 linear_accel;
            Vec3 gravity;
            Quaternion quat;
          };

    /**
    * @class Bno055
    * @note Provides initialization, sensor reads, calibration, and diagnostics (self-test).
    * Orientation is represented as a **quarternion** to avoid gimbal lock - Nick Fan.
    */

    class Bno055 {
    public:

        //I2C Addresses
        static constexpr uint8_t ADDR_PRIMARY   = 0x28;   ///< Default I2C Address 
        static constexpr uint8_t ADDR_ALTERNATE = 0x29;   ///< Alternate I2C Address

        //Common BN055 registers
        struct SysReg {
            static constexpr uint8_t CHIP_ID      = 0x00;
            static constexpr uint8_t OPR_MODE     = 0x3D;
            static constexpr uint8_t PWR_MODE     = 0x3E;
            static constexpr uint8_t PAGE_ID      = 0x07;
            static constexpr uint8_t SYS_TRIGGER  = 0x3F;

            static constexpr uint8_t ST_RESULT    = 0x36; // POST/BIST result bits
            static constexpr uint8_t CALIB_STAT   = 0x35;
            static constexpr uint8_t SYS_STATUS   = 0x39;
            static constexpr uint8_t SYS_ERR      = 0x3A;   
        };

        // Data Registers

        /* Start Registers*/
        struct RegStart {
            static constexpr uint8_t ACC    = 0x08; 
            static constexpr uint8_t GYR    = 0x14; 
            static constexpr uint8_t QUAT   = 0x20; // < 8 bytes
            static constexpr uint8_t LIA    = 0x28;
            static constexpr uint8_t GRAV   = 0x2E;
        };
        
        /* Calibration offsets (optional) */
        struct OffsetReg {
            // Accelerometer offsets
            static constexpr uint8_t ACC_X_LSB = 0x55;
            static constexpr uint8_t ACC_X_MSB = 0x56;
            static constexpr uint8_t ACC_Y_LSB = 0x57;
            static constexpr uint8_t ACC_Y_MSB = 0x58;
            static constexpr uint8_t ACC_Z_LSB = 0x59;
            static constexpr uint8_t ACC_Z_MSB = 0x5A;

            // Gyroscope offsets
            static constexpr uint8_t GYR_X_LSB = 0x61;
            static constexpr uint8_t GYR_X_MSB = 0x62;
            static constexpr uint8_t GYR_Y_LSB = 0x63;
            static constexpr uint8_t GYR_Y_MSB = 0x64;
            static constexpr uint8_t GYR_Z_LSB = 0x65;
            static constexpr uint8_t GYR_Z_MSB = 0x66;
        };

        /* Linear acceleration & Gravity vector registers */
        struct LinearReg {
            static constexpr uint8_t X_LSB = 0x28;
            static constexpr uint8_t X_MSB = 0x29;
            static constexpr uint8_t Y_LSB = 0x2A;
            static constexpr uint8_t Y_MSB = 0x2B;
            static constexpr uint8_t Z_LSB = 0x2C;
            static constexpr uint8_t Z_MSB = 0x2D;
        };
        struct GravityReg {
            static constexpr uint8_t X_LSB = 0x2E;
            static constexpr uint8_t X_MSB = 0x2F;
            static constexpr uint8_t Y_LSB = 0x30;
            static constexpr uint8_t Y_MSB = 0x31;
            static constexpr uint8_t Z_LSB = 0x32;
            static constexpr uint8_t Z_MSB = 0x33;
        };


        // Axis remap (if needed)
        static constexpr uint8_t AXIS_REMAP_CONFIG = 0x24;

        // Operating & Fusion Modes
        struct ModeReg {
            static constexpr uint8_t CONFIG        = 0x00;
            static constexpr uint8_t IMU           = 0x08;
            static constexpr uint8_t NDOF          = 0x0C;
        };

        // Power modes
        struct PowerReg {
            static constexpr uint8_t NORMAL     = 0x00;
            static constexpr uint8_t LOW_POWER  = 0x01;
            static constexpr uint8_t SUSPEND    = 0x02;

        };

        enum class Mode : uint8_t {
            CONFIG = ModeReg::CONFIG,
            IMU    = ModeReg::IMU,
            NDOF   = ModeReg::NDOF
        };
    
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


        explicit Bno055(LBR::Stml4::HwI2c& i2c, uint8_t addr = ADDR_PRIMARY);
        Bno055(LBR::Stml4::HwI2c& i2c, uint8_t addr, I2cMemReadFn read_fn, I2cMemWriteFn write_fn);
        // Public API
        /** Initialize and configure the IMU */
        void init();
        void deinit();

        // Unified snapshot read
        bool read_all(Bno055Data& out);

        // Testing Purpose 
        uint8_t get_chip_id(uint8_t& id);

        // Status & Diagnosis
        uint8_t calibrate();
        uint8_t get_sys_status();
        uint8_t get_sys_error();
        uint8_t run_post();
        uint8_t run_bist();

    private:

        // I2C communication methods or helpers
       void write_reg(uint8_t reg, uint8_t value);
       uint8_t read_reg(uint8_t reg);
       void read_multi(uint8_t startReg, uint8_t* buffer, size_t len);
       void set_mode(Mode mode);
        
       // I2C interface and device address
        LBR::Stml4::HwI2c& i2c_;
        uint8_t address_;
        I2cMemReadFn mem_read_fn_;   // Optional callback for repeated start reads
        I2cMemWriteFn mem_write_fn_; // Optional callback for writes
    };
} // namespace LBR