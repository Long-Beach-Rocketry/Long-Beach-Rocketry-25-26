/**
 * @file ms561101.h
 * @brief Declares the MS5611 barometric pressure and temperature sensor driver.
 * @author Hai Phan
 * @date 10/31/2026
 */

#pragma once
#include <cstdint>
#include "i2c.h"

namespace LBR
{

struct Ms561101Params
{
    I2c& i2c;
    uint8_t dev_addr;
};

struct Ms561101CalibCoeffs
{
    /*
    C1: Pressure sensitivity
    C2: Pressure offset
    C3: Temperature coefficient of pressure sensitivity
    C4: Temperature coefficient of pressure offset
    C5: Reference temperature
    C6: Temperature coefficient of temperature
    */
    uint16_t c1{};
    uint16_t c2{};
    uint16_t c3{};
    uint16_t c4{};
    uint16_t c5{};
    uint16_t c6{};
};

struct Ms561101RawData
{
    /*
    D1: Digital pressure value
    D2: Digital temperature value
    */
    uint32_t d1{};
    uint32_t d2{};
};

class Ms561101
{
public:
    explicit Ms561101(const Ms561101Params& params);

    bool init(void);
    bool get_pressure(float& pressure);
    bool get_temperature(float& temperature);
    ~Ms561101() = default;

private:
    I2c& _i2c;
    //sensor I2C address
    uint8_t _dev_addr;
    Ms561101CalibCoeffs _calib_coeffs{};
    Ms561101RawData _raw_data{};

     /**
     * @brief Reads the factory calibration coefficients from the sensor PROM.
     * @return true if all coefficients are read successfully; otherwise, false.
     */
    bool get_calib_coeffs(void);

    /**
     * @brief Reads the raw pressure (D1) and temperature (D2) values from the sensor.
     * @return true if both raw values are read successfully; otherwise, false.
     */
    bool get_raw_data(void);

    /**
     * @brief Calculates compensated pressure using the raw readings and calibration coefficients.
     * @return The compensated pressure value.
     */
    float compensate_pressure(void);

    /**
     * @brief Calculates compensated temperature using the raw readings and calibration coefficients.
     * @return The compensated temperature in degrees Celsius.
     */
    float compensate_temperature(void);

    /**
     * @brief Reads one calibration coefficient from the specified PROM address.
     * @param address PROM address of the coefficient to read.
     * @param value Receives the coefficient value.
     * @return true if the coefficient is read successfully; otherwise, false.
     */
    bool read_coeff(uint8_t address, uint16_t& value);
};

}  // namespace LBR