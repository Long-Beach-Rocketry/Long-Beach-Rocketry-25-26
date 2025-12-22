/**
 * @file bno055_imu.h
 * @brief BNO055 IMU sensor interface
 * @author Bex Saw
 * @date 2025-10-22
 * @note This file contains the interface for the BNO055 IMU sensor, including initialization, reading, and calibration functions.
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "imu_math.h"

namespace LBR
{
class I2c;
}  // namespace LBR

namespace LBR::Stml4
{
class HwI2c;
}  // namespace LBR::Stml4

namespace LBR
{

// Math types moved to imu_math.h
struct Bno055Data
{
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
class Bno055
{
public:
    // IMU mode constants
    static constexpr uint8_t MODE_REG_CONFIG = 0x00;
    static constexpr uint8_t MODE_REG_IMU = 0x08;
    static constexpr uint8_t MODE_REG_NDOF = 0x0C;

    static constexpr uint8_t ADDR_PRIMARY = 0x28;    ///< Default I2C Address
    static constexpr uint8_t ADDR_ALTERNATE = 0x29;  ///< Alternate I2C Address

    explicit Bno055(LBR::I2c& i2c, uint8_t addr = ADDR_PRIMARY);

    /**
   * @brief Get the current IMU operating mode (OPR_MODE register)
   * @param[out] mode Output variable for mode value
   * @return true if successful, false otherwise
   */
    bool get_opr_mode(uint8_t& mode);

    /**
   * @brief Set the IMU operating mode.
   * @param mode Mode constant
   * @return true if successful, false otherwise
   */
    bool set_mode(uint8_t mode);

    /**
   * @brief Initialize and configure the IMU
   */
    void init();

    /**
   * @brief Deinitialize the IMU and put it in low-power mode
   */
    void deinit();

    /**
   * @brief Read all sensor data from the IMU
   * @param[out] out Output struct for sensor data
   * @return true if successful, false otherwise
   */
    bool read_all(Bno055Data& out);

    /**
   * @brief Read the IMU chip ID
   * @param[out] id Output chip ID
   * @return true if successful, false otherwise
   */
    bool get_chip_id(uint8_t& id);

    /**
   * @brief Get IMU calibration status
   * @param[out] value Output calibration status
   * @return true if successful, false otherwise
   */
    bool calibrate(uint8_t& value);

    /**
   * @brief Get IMU system status
   * @param[out] value Output system status
   * @return true if successful, false otherwise
   */
    bool get_sys_status(uint8_t& value);

    /**
   * @brief Get IMU system error
   * @param[out] value Output system error
   * @return true if successful, false otherwise
   */
    bool get_sys_error(uint8_t& value);

    /**
   * @brief Run IMU power-on self-test
   * @param[out] status Output self-test result
   * @return true if successful, false otherwise
   */
    bool run_post(uint8_t& status);

    /**
   * @brief Run IMU built-in self-test
   * @param[out] status Output BIST result
   * @return true if successful, false otherwise
   */
    bool run_bist(uint8_t& status);

    // I2C interface and device address
    LBR::I2c& i2c_;
    uint8_t address_;
};

}  // namespace LBR