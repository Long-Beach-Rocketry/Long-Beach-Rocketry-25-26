#include "ms561101.h"
#include <array>

#include "reg_helpers.h"
#include "delay.h"

/*Register and bit definitions*/
static constexpr uint8_t REST{0x1E};
static constexpr uint8_t PROM{0xA0};
static constexpr uint8_t D1{0x40};
static constexpr uint8_t D2{0x50};
static constexpr uint8_t ADC_result{0x00};

/*Conversion Factors For Coefficients: Read the data sheet*/
static constexpr uint32_t C1_CONV_FACT{32768};  // 2^15
static constexpr uint32_t C2_CONV_FACT{65536};  // 2^16
static constexpr uint32_t C3_CONV_FACT{256};    // 2^8
static constexpr uint32_t C4_CONV_FACT{128};    // 2^7
static constexpr uint32_t C5_CONV_FACT{256};    // 2^8
static constexpr uint32_t C6_CONV_FACT{8388608};  // 2^23

namespace LBR
{
ms561101::ms561101(const ms561101Params& params)
    : _i2c{params.i2c},
      _dev_addr{params.dev_addr},
      _calib_coeffs{},
      _raw_data{}
{
}

float ms561101::get_pressure()
{
    get_raw_data();
    return compensate_pressure();
}

float ms561101::get_temperature()
{
    get_raw_data();
    return compensate_temperature();
}

// read each individually one at the time
std::uint16_t ms561101::read_coeff(std::uint8_t address)
{
    std::array<std::uint8_t, 2> data{};
    /*data`: store the returned bytes here
      address`: request C1, C2, etc.
      dev_addr`: communicate with this MS5611 device*/
    _i2c.mem_read(data, address, _dev_addr);
    return combine_uint16(data[0], data[1]);
}

bool ms561101::get_calib_coeffs(void)
{
    // read the calibration data
    // the read coefficient is 0xA0 + shift left one
    const std::uint16_t c1 = read_coeff(0xA2);
    const std::uint16_t c2 = read_coeff(0xA4);
    const std::uint16_t c3 = read_coeff(0xA6);
    const std::uint16_t c4 = read_coeff(0xA8);
    const std::uint16_t c5 = read_coeff(0xAA);
    const std::uint16_t c6 = read_coeff(0xAC);

    // Store the coefficients
    _calib_coeffs.c1 = c1;
    _calib_coeffs.c2 = c2;
    _calib_coeffs.c3 = c3;
    _calib_coeffs.c4 = c4;
    _calib_coeffs.c5 = c5;
    _calib_coeffs.c6 = c6;

    return true;
}

bool ms561101::get_raw_data(void)
{
    // read(): recieve the byte from I2C
    // write(): send bytes froms Stm32 to sensor
    // the I2C does not accept one byte, need the group of byte
    std::array<std::uint8_t, 1> command{D1};
    _i2c.write(command, _dev_addr);
    Utils::DelayMs(2);

    // 24 bits of adcs = 3 bytes
    std::array<std::uint8_t, 3> pressure{};
    _i2c.mem_read(pressure, ADC_result, _dev_addr);

    // change to D2
    command[0] = D2;
    _i2c.write(command, _dev_addr);
    Utils::DelayMs(2);

    // get the temperature
    std::array<std::uint8_t, 3> temperature{};
    _i2c.mem_read(temperature, ADC_result, _dev_addr);

    _raw_data.d1 = (static_cast<std::uint32_t>(pressure[0]) << 16) |
                   (static_cast<std::uint32_t>(pressure[1]) << 8) |
                   pressure[2];

    _raw_data.d2 = (static_cast<std::uint32_t>(temperature[0]) << 16) |
                   (static_cast<std::uint32_t>(temperature[1]) << 8) |
                   temperature[2];
    return true;
}

float ms561101::compensate_pressure(void)
{
    // static_cast: which number type to use for a calculation.
    const std::int64_t dT{
        static_cast<std::int64_t>(_raw_data.d2) -
        static_cast<std::int64_t>(_calib_coeffs.c5) * C5_CONV_FACT};
    const std::int64_t temp =
        2000 + dT * _calib_coeffs.c6 / C6_CONV_FACT;  // object "_calib_coeff" call the c6

    std::int64_t off{
        static_cast<std::int64_t>(_calib_coeffs.c2) * C2_CONV_FACT +
        static_cast<std::int64_t>(_calib_coeffs.c4) * dT / C4_CONV_FACT};
    std::int64_t sens{
        static_cast<std::int64_t>(_calib_coeffs.c1) * C1_CONV_FACT +
        static_cast<std::int64_t>(_calib_coeffs.c3) * dT / C3_CONV_FACT};
    if (temp < 2000)
    {
        const std::int64_t delta = temp - 2000;
        std::int64_t off2 = 5 * delta * delta / 2;
        std::int64_t sens2 = 5 * delta * delta / 4;

        if (temp < -1500)
        {
            const std::int64_t cold_delta = temp + 1500;
            off2 += 7 * cold_delta * cold_delta;
            sens2 += 11 * cold_delta * cold_delta / 2;
        }

        off -= off2;
        sens -= sens2;
    }

    const std::int64_t pressure =
        (static_cast<std::int64_t>(_raw_data.d1) * sens / 2097152 - off) /
        32768;

    return static_cast<float>(pressure);
}

float ms561101::compensate_temperature(void)
{
    const std::int64_t dT{
        static_cast<std::int64_t>(_raw_data.d2) -
        static_cast<std::int64_t>(_calib_coeffs.c5) * C5_CONV_FACT};
    std::int64_t temp{2000 + dT * _calib_coeffs.c6 / C6_CONV_FACT};
    if (temp < 2000)
    {
        const std::int64_t T2 = dT * dT / 2147483648LL;  // 2^31
        temp = temp - T2;
    }

    return static_cast<float>(temp) / 100.0f;
}

bool ms561101::init()
{
    std::array<std::uint8_t, 1> command{REST};
    if (!_i2c.write(command, _dev_addr))
    {
        return false;
    }

    Utils::DelayMs(3);
    return get_calib_coeffs();
}
}  // namespace LBR

