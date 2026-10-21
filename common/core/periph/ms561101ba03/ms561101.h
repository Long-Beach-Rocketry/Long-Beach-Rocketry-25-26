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
    // C1: Pressure sensitivity
    // C2: Pressure offset
    // C3: Temperature coefficient of pressure sensitivity
    // C4: Temperature coefficient of pressure offset
    // C5: Reference temperature
    // C6: Temperature coefficient of temperature

    uint16_t c1{};
    uint16_t c2{};
    uint16_t c3{};
    uint16_t c4{};
    uint16_t c5{};
    uint16_t c6{};
};

struct Ms561101RawData
{
    // D1: Digital pressure value
    // D2: Digital temperature value
    uint32_t d1{};
    uint32_t d2{};
};

class Ms561101
{
public:
    explicit Ms561101(const Ms561101Params& params);

    bool init();
    float get_pressure();
    float get_temperature();

private:
    I2c& _i2c;
    //sensor I2C address
    uint8_t _dev_addr;
    Ms561101CalibCoeffs _calib_coeffs{};
    Ms561101RawData _raw_data{};
    
    //function
    bool get_calib_coeffs(void);
    bool get_raw_data(void);
    float compensate_pressure(void);
    float compensate_temperature(void);
    uint16_t read_coeff(uint8_t address);
};

} // namespace LBR