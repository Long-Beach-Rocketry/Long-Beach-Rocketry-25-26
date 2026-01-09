#include "bmp390.h"
#include <array>
#include "reg_helpers.h"

/*** BMP390 Registers and Bit Definitions ***/
static constexpr uint8_t PWR_CTRL_REG{0x1B};
static constexpr uint8_t DATA_REG_START{0x04};
static constexpr uint8_t CALIB_REG_START{0x31};

static constexpr uint8_t PWR_CTRL_PRESS_EN{0x1};
static constexpr uint8_t PWR_CTRL_TEMP_EN{0x2};
static constexpr uint8_t PWR_CTRL_NORMAL_MODE{0x3 << 4};
static constexpr uint8_t DATA_REG_LEN{6};
static constexpr uint8_t CALIB_REG_LEN{21};

/*** Conversion Factors For Coefficients ***/
static constexpr float T1_P7_CONV_FACT{256.0};
static constexpr float T2_CONV_FACT{1073741824.0};
static constexpr float T3_P9_P10_CONV_FACT{281474976710656.0};
static constexpr float P1_P2_CONV_FACT{16384.0};
static constexpr float P1_CONV_FACT{1048576.0};
static constexpr float P2_CONV_FACT{536870912.0};
static constexpr float P3_CONV_FACT{4294967296.0};
static constexpr float P4_CONV_FACT{137438953472.0};
static constexpr float P5_CONV_FACT{8.0};
static constexpr float P6_CONV_FACT{64.0};
static constexpr float P8_CONV_FACT{32768.0};
static constexpr float P11_CONV_FACT{36893488147419103232.0};

namespace LBR
{
Bmp390::Bmp390(const Bmp390Params& params)
    : _i2c{params.i2c},
      _dev_addr{params.dev_addr},
      _calib_coeffs{0},
      _raw_data{0}
{
}

void Bmp390::get_calib_coeffs()
{
    // Read calibration data
    std::array<uint8_t, CALIB_REG_LEN> data{0};
    _i2c.mem_read(data, CALIB_REG_START, _dev_addr);

    // Store coefficients
    uint16_t t1 = combine_uint16(data[1], data[0]);
    uint16_t t2 = combine_uint16(data[3], data[2]);
    int8_t t3 = static_cast<int8_t>(data[4]);
    int16_t p1 = combine_int16(data[6], data[5]);
    int16_t p2 = combine_int16(data[8], data[7]);
    int8_t p3 = static_cast<int8_t>(data[9]);
    int8_t p4 = static_cast<int8_t>(data[10]);
    uint16_t p5 = combine_uint16(data[12], data[11]);
    uint16_t p6 = combine_uint16(data[14], data[13]);
    int8_t p7 = static_cast<int8_t>(data[15]);
    int8_t p8 = static_cast<int8_t>(data[16]);
    int16_t p9 = combine_int16(data[18], data[17]);
    int8_t p10 = static_cast<int8_t>(data[19]);
    int8_t p11 = static_cast<int8_t>(data[20]);

    // Convert coefficients to floating point numbers
    _calib_coeffs.t1 = static_cast<float>(t1) * T1_P7_CONV_FACT;
    _calib_coeffs.t2 = static_cast<float>(t2) / T2_CONV_FACT;
    _calib_coeffs.t3 = static_cast<float>(t3) / T3_P9_P10_CONV_FACT;
    _calib_coeffs.p1 =
        (static_cast<float>(p1) - P1_P2_CONV_FACT) / P1_CONV_FACT;
    _calib_coeffs.p2 =
        (static_cast<float>(p2) - P1_P2_CONV_FACT) / P2_CONV_FACT;
    _calib_coeffs.p3 = static_cast<float>(p3) / P3_CONV_FACT;
    _calib_coeffs.p4 = static_cast<float>(p4) / P4_CONV_FACT;
    _calib_coeffs.p5 = static_cast<float>(p5) * P5_CONV_FACT;
    _calib_coeffs.p6 = static_cast<float>(p6) / P6_CONV_FACT;
    _calib_coeffs.p7 = static_cast<float>(p7) / T1_P7_CONV_FACT;
    _calib_coeffs.p8 = static_cast<float>(p8) / P8_CONV_FACT;
    _calib_coeffs.p9 = static_cast<float>(p9) / T3_P9_P10_CONV_FACT;
    _calib_coeffs.p10 = static_cast<float>(p10) / T3_P9_P10_CONV_FACT;
    _calib_coeffs.p11 = static_cast<float>(p11) / P11_CONV_FACT;
}

void Bmp390::get_raw_data()
{
    // Get raw sensor readings
    std::array<uint8_t, DATA_REG_LEN> data{0};
    _i2c.mem_read(data, DATA_REG_START, _dev_addr);

    // Store data
    _raw_data.press = (static_cast<uint32_t>(data[2]) << 16) |
                      (static_cast<uint32_t>(data[1]) << 8) |
                      static_cast<uint32_t>(data[0]);
    _raw_data.temp = (static_cast<uint32_t>(data[5]) << 16) |
                     (static_cast<uint32_t>(data[4]) << 8) |
                     static_cast<uint32_t>(data[3]);
}

float Bmp390::compensate_temperature()
{
    float partial_data1;
    float partial_data2;

    // Calculate compensated temperature
    partial_data1 = static_cast<float>(_raw_data.temp) - _calib_coeffs.t1;
    partial_data2 = partial_data1 * _calib_coeffs.t2;

    // Store in coeff struct to be used for compensated pressure calculation
    _calib_coeffs.t_lin =
        partial_data2 + (partial_data1 * partial_data1) * _calib_coeffs.t3;

    return _calib_coeffs.t_lin;
}

float Bmp390::compensate_pressure()
{
    float partial_data1;
    float partial_data2;
    float partial_data3;
    float partial_data4;
    float partial_out1;
    float partial_out2;

    partial_data1 = _calib_coeffs.p6 * _calib_coeffs.t_lin;
    partial_data2 =
        _calib_coeffs.p7 * (_calib_coeffs.t_lin * _calib_coeffs.t_lin);
    partial_data3 =
        _calib_coeffs.p8 *
        (_calib_coeffs.t_lin * _calib_coeffs.t_lin * _calib_coeffs.t_lin);
    partial_out1 =
        _calib_coeffs.p5 + partial_data1 + partial_data2 + partial_data3;

    partial_data1 = _calib_coeffs.p2 * _calib_coeffs.t_lin;
    partial_data2 =
        _calib_coeffs.p3 * (_calib_coeffs.t_lin * _calib_coeffs.t_lin);
    partial_data3 =
        _calib_coeffs.p4 *
        (_calib_coeffs.t_lin * _calib_coeffs.t_lin * _calib_coeffs.t_lin);
    partial_out2 =
        static_cast<float>(_raw_data.press) *
        (_calib_coeffs.p1 + partial_data1 + partial_data2 + partial_data3);

    partial_data1 = static_cast<float>(_raw_data.press) *
                    static_cast<float>(_raw_data.press);
    partial_data2 = _calib_coeffs.p9 + _calib_coeffs.p10 * _calib_coeffs.t_lin;
    partial_data3 = partial_data1 * partial_data2;
    partial_data4 = partial_data3 + (static_cast<float>(_raw_data.press) *
                                     static_cast<float>(_raw_data.press) *
                                     static_cast<float>(_raw_data.press)) *
                                        _calib_coeffs.p11;

    return partial_out1 + partial_out2 + partial_data4;
}

bool Bmp390::init()
{
    // Configure power mode to Normal Mode and enable temp and press measurements
    bool status;
    std::array<uint8_t, 1> config{PWR_CTRL_NORMAL_MODE | PWR_CTRL_TEMP_EN |
                                  PWR_CTRL_PRESS_EN};
    status = _i2c.mem_write(config, PWR_CTRL_REG, _dev_addr);

    // Get calibration coefficients
    get_calib_coeffs();

    return status;
}

float Bmp390::get_pressure()
{
    get_raw_data();
    compensate_temperature();  // calculated pressure relies on calculated temperature
    return compensate_pressure();
}

float Bmp390::get_temperature()
{
    get_raw_data();
    return compensate_temperature();
}
}  // namespace LBR