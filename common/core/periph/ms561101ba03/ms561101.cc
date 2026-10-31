#include "ms561101.h"
#include <array>
#include "delay.h"
#include "reg_helpers.h"
/*Register and bit definitions*/
static constexpr uint8_t REST{0x1E};
static constexpr uint8_t PROM{0xA0};
static constexpr uint8_t D1{0x40};
static constexpr uint8_t D2{0x50};
static constexpr uint8_t ADC_result{0x00};

/*Conversion Factors For Coefficients: Read the data sheet*/
static constexpr uint32_t C1_CONV_FACT{32768};    // 2^15
static constexpr uint32_t C2_CONV_FACT{65536};    // 2^16
static constexpr uint32_t C3_CONV_FACT{256};      // 2^8
static constexpr uint32_t C4_CONV_FACT{128};      // 2^7
static constexpr uint32_t C5_CONV_FACT{256};      // 2^8
static constexpr uint32_t C6_CONV_FACT{8388608};  // 2^23

namespace LBR
{
Ms561101::Ms561101(const Ms561101Params& params)
    : _i2c{params.i2c},
      _dev_addr{params.dev_addr},
      _calib_coeffs{0},
      _raw_data{0}
{
}

bool Ms561101::get_temperature(float& temperature)
{
    if (!get_raw_data())
    {
        return false;
    }

    temperature = compensate_temperature();
    return true;
}

bool Ms561101::get_pressure(float& pressure)
{
    if (!get_raw_data())
    {
        return false;
    }

    pressure = compensate_pressure();
    return true;
}

// read each individually one at the time
bool Ms561101::read_coeff(uint8_t address, uint16_t& value)
{
    std::array<uint8_t, 2> data{};
    /*data`: store the returned bytes here
      address`: request C1, C2, etc.
      dev_addr`: communicate with this MS5611 device*/
    if (!_i2c.mem_read(data, address, _dev_addr))
    {
        return false;
    }
    value = combine_uint16(data[0], data[1]);
    return true;
}

bool Ms561101::get_calib_coeffs(void)
{
    Ms561101CalibCoeffs coeffs{};
    if (!read_coeff(0xA2, coeffs.c1) || !read_coeff(0xA4, coeffs.c2) ||
        !read_coeff(0xA6, coeffs.c3) || !read_coeff(0xA8, coeffs.c4) ||
        !read_coeff(0xAA, coeffs.c5) || !read_coeff(0xAC, coeffs.c6))
    {
        return false;
    }

    _calib_coeffs = coeffs;
    return true;
}

bool Ms561101::get_raw_data(void)
{
    /*
    read(): recieve the byte from I2C
    write(): send bytes froms Stm32 to sensor
    the I2C does not accept one byte, need the group of byte
    */
    std::array<std::uint8_t, 1> command{D1};

    if (!_i2c.write(command, _dev_addr))
    {
        return false;
    }

    LBR::Utils::DelayMs(3);

    // 24 bits of adcs = 3 bytes
    std::array<std::uint8_t, 3> pressure{};
    if (!_i2c.mem_read(pressure, ADC_result, _dev_addr))
    {
        return false;
    }

    // change to D2
    command[0] = D2;
    if (!_i2c.write(command, _dev_addr))
    {
        return false;
    }

    LBR::Utils::DelayMs(3);

    // get the temperature
    std::array<uint8_t, 3> temperature{};
    if (!_i2c.mem_read(temperature, ADC_result, _dev_addr))
    {
        return false;
    }

    _raw_data.d1 = (static_cast<uint32_t>(pressure[0]) << 16) |
                   (static_cast<uint32_t>(pressure[1]) << 8) | pressure[2];

    _raw_data.d2 = (static_cast<uint32_t>(temperature[0]) << 16) |
                   (static_cast<uint32_t>(temperature[1]) << 8) |
                   temperature[2];
    return true;
}
/**
 * Pressure compensation formula:
 * dT   = D2 - C5 × 2^8
 * TEMP = 2000 + dT × C6 / 2^23
 * OFF  = C2 × 2^16 + C4 × dT / 2^7
 * SENS = C1 × 2^15 + C3 × dT / 2^8
 *
 * If TEMP < 2000, apply the low-temperature corrections to OFF and SENS.
 * P = (D1 × SENS / 2^21 - OFF) / 2^15
 */
float Ms561101::compensate_pressure(void)
{
    // static_cast: which number type to use for a calculation.
    const int64_t dT{static_cast<int64_t>(_raw_data.d2) -
                     static_cast<int64_t>(_calib_coeffs.c5) * C5_CONV_FACT};
    const int64_t temp =
        2000 + dT * _calib_coeffs.c6 /
                   C6_CONV_FACT;  // object "_calib_coeff" call the c6

    int64_t off{static_cast<int64_t>(_calib_coeffs.c2) * C2_CONV_FACT +
                static_cast<int64_t>(_calib_coeffs.c4) * dT / C4_CONV_FACT};
    int64_t sens{static_cast<int64_t>(_calib_coeffs.c1) * C1_CONV_FACT +
                 static_cast<int64_t>(_calib_coeffs.c3) * dT / C3_CONV_FACT};
    if (temp < 2000)
    {
        const int64_t delta = temp - 2000;
        int64_t off2 = 5 * delta * delta / 2;
        int64_t sens2 = 5 * delta * delta / 4;

        if (temp < -1500)
        {
            const int64_t cold_delta = temp + 1500;
            off2 += 7 * cold_delta * cold_delta;
            sens2 += 11 * cold_delta * cold_delta / 2;
        }

        off -= off2;
        sens -= sens2;
    }

    const float pressure =
        (static_cast<float>(_raw_data.d1) * sens / 2097152 - off) / 32768;

    return static_cast<float>(pressure);
}
/**
 * Temperature compensation formula:
 * dT   = D2 - C5 × 2^8
 * TEMP = 2000 + dT × C6 / 2^23
 *
 * If TEMP < 2000, apply the second-order temperature correction.
 * Return TEMP / 100 as degrees Celsius.
 */
float Ms561101::compensate_temperature(void)
{
    const int64_t dT{static_cast<int64_t>(_raw_data.d2) -
                     static_cast<int64_t>(_calib_coeffs.c5) * C5_CONV_FACT};
    int64_t temp{2000 + dT * _calib_coeffs.c6 / C6_CONV_FACT};
    if (temp < 2000)
    {
        const int64_t T2 = dT * dT / 2147483648LL;  // 2^31
        temp = temp - T2;
    }

    return static_cast<float>(temp) / 100.0f;
}

bool Ms561101::init()
{
    std::array<uint8_t, 1> command{REST};
    if (!_i2c.write(command, _dev_addr))
    {
        return false;
    }
    LBR::Utils::DelayMs(3);

    return get_calib_coeffs();
}
}  // namespace LBR
