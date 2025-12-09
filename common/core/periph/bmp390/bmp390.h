/**
 * @file bmp390.h
 * @brief Barometric pressure sensor driver for BMP390
 * @author Yshi Blanco
 * @date 11/24/2025
 */

#pragma once
#include <cstdint>
#include <span>
#include "i2c.h"

/*** BMP390 Registers and Bit Definitions ***/
inline constexpr uint8_t PWR_CTRL_REG = (0x1B);
inline constexpr uint8_t DATA_REG_START = (0x04);
inline constexpr uint8_t CALIB_REG_START = (0x31);

inline constexpr uint8_t PWR_CTRL_PRESS_EN = (0x1);
inline constexpr uint8_t PWR_CTRL_TEMP_EN = (0x2);
inline constexpr uint8_t PWR_CTRL_NORMAL_MODE = (0x3 << 4);
inline constexpr uint8_t DATA_REG_LEN = (6);
inline constexpr uint8_t CALIB_REG_LEN = (21);

namespace LBR
{
/**
 * @brief Collection of I2C instance and device address information
 */
struct Bmp390Params
{
    I2c& i2c;
    uint8_t dev_addr;
};

/**
 * @brief Collection of calibration coefficients to be used in compensation formulas
 */
struct Bmp390CalibCoeffs
{
    // temperature coefficients
    float t1;
    float t2;
    float t3;

    // pressure coefficients
    float p1;
    float p2;
    float p3;
    float p4;
    float p5;
    float p6;
    float p7;
    float p8;
    float p9;
    float p10;
    float p11;

    // compensated temp to be used in pressure calculation
    float t_lin;
};

struct Bmp390RawData
{
    uint32_t press;
    uint32_t temp;
};

class Bmp390
{
public:
    /**
     * @brief BMP390 Contructor
     * @param params Struct containing I2C interface and device address.
     */
    explicit Bmp390(const Bmp390Params& params);

    /**
     * @brief BMP390 hardware initializer
     * @return true if successful, false otherwise
     */
    bool init(void);

    /**
     * @brief Get compensated pressure data from sensor
     * @return Pressure in Pascals
     */
    float get_pressure(void);

    /**
     * @brief Get compensated temperature data from sensor
     * @return Temperature in degrees Celsius
     */
    float get_temperature(void);

    ~Bmp390() = default;

private:
    I2c& _i2c;
    uint8_t _dev_addr;
    Bmp390CalibCoeffs _calib_coeffs;  // zero-initialized until init
    Bmp390RawData _raw_data;          // zero-initialized until first read

    /**
     * @brief Get calibration coefficients for data compensation formulas
     */
    void get_calib_coeffs(void);

    /**
     * @brief Get raw pressure and temperature data from sensor
     */
    void get_raw_data(void);

    /**
     * @brief Calculates real pressure value
     * @return Compensated pressure value
     */
    float compensate_pressure(void);

    /**
     * @brief Calculates real temperature value
     * @return Compensated temperature value
     */
    float compensate_temperature(void);

    /**
     * @brief Combines two bytes into a 16-bit value
     * @return 16-bit unsigned integer
     */
    uint16_t combine_uint16(uint8_t msb, uint8_t lsb);

    /**
     * @brief Combines two bytes into a 16-bit signed value
     * @return 16-bit signed integer
     */
    int16_t combine_int16(uint8_t msb, uint8_t lsb);
};
}  // namespace LBR