#pragma once
#include <cstdint>
#include "i2c.h"

namespace LBR
{

struct ms561101Params
{
    I2c& i2c;
    std::uint8_t dev_addr;
};

struct ms561101CalibCoeffs
{
    // C1: Pressure sensitivity
    // C2: Pressure offset
    // C3: Temperature coefficient of pressure sensitivity
    // C4: Temperature coefficient of pressure offset
    // C5: Reference temperature
    // C6: Temperature coefficient of temperature

    std::uint16_t c1{};
    std::uint16_t c2{};
    std::uint16_t c3{};
    std::uint16_t c4{};
    std::uint16_t c5{};
    std::uint16_t c6{};
};

struct ms561101RawData
{
    // D1: Digital pressure value
    // D2: Digital temperature value
    std::uint32_t d1{};
    std::uint32_t d2{};
};

class ms561101
{
public:
    explicit ms561101(const ms561101Params& params);

    bool init();
    float get_pressure();
    float get_temperature();

private:
    I2c& _i2c;
    //sensor I2C address
    std::uint8_t _dev_addr;
    ms561101CalibCoeffs _calib_coeffs{};
    ms561101RawData _raw_data{};
    
    //function
    bool get_calib_coeffs(void);
    bool get_raw_data(void);
    float compensate_pressure(void);
    float compensate_temperature(void);
    std::uint16_t read_coeff(std::uint8_t address);
};

} // namespace LBR