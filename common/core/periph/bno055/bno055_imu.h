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
#include "delay.h"
#include "i2c.h"
#include "imu_math.h"

namespace LBR
{

/**
 * @struct Bno055Data
 * @brief Sensor data from BNO055 IMU
 * @details
 *  - accel: Acceleration (m/s^2)
 *  - gyro: Angular velocity (rad/s)
 *  - linear_accel: Linear acceleration (m/s^2, gravity removed)
 *  - gravity: Gravity vector (m/s^2)
 *  - quat: Orientation (unit quaternion)
 */
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
 * @brief BNO055 IMU interface (generic over I2c)
 * @details Provides initialization, sensor reads, calibration, and diagnostics (self-test).
 * Orientation is represented as a quaternion (unitless, normalized).
 */
class Bno055
{

public:
    /**
     * @enum Mode
     * @brief IMU operating modes
     */
    enum Mode : uint8_t
    {
        CONFIG = 0x00,
        IMU = 0x08,
        NDOF = 0x0C
    };

    static constexpr uint8_t REG_OPR_MODE =
        0x3D;  ///< OPR_MODE register address

    static constexpr uint8_t ADDR_PRIMARY = 0x28;    ///< Default I2C Address
    static constexpr uint8_t ADDR_ALTERNATE = 0x29;  ///< Alternate I2C Address

    /**
     * @brief Construct a new Bno055 object
     * @param i2c Reference to I2c interface
     * @param addr I2C address (default: ADDR_PRIMARY)
     */
    explicit Bno055(LBR::I2c& i2c, uint8_t addr = ADDR_PRIMARY);

    /**
     * @brief Set the IMU operating mode.
     * @param mode Mode enum
     * @return true if successful, false otherwise
     */
    bool set_mode(Mode mode);

    /**
     * @brief Initialize and configure the IMU
     */
    void init();

    /**
     * @brief Deinitialize the IMU and put it in low-power mode
     * @note @TJMalaska Check this function
     */
    void deinit();

    /**
     * @brief Read all sensor data from the IMU
     * @param[out] out Output struct for sensor data
     * @return true if successful, false otherwise
     */
    bool read_all(Bno055Data& out);

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

    /**
     * @brief Read the IMU chip ID
     * @param[out] id Output chip ID
     * @return true if successful, false otherwise
     */
    bool get_chip_id(uint8_t& id);

    /**
     * @brief Get the current IMU operating mode (OPR_MODE register)
     * @param[out] mode Output variable for mode enum
     * @return true if successful, false otherwise
     */
    bool get_opr_mode(Mode& mode);

private:
    LBR::I2c& i2c_;    ///< Reference to I2c interface
    uint8_t address_;  ///< I2C address
};

}  // namespace LBR