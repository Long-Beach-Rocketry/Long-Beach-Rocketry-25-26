#include <cmath>
#include "airbrake_constants.h"

using namespace LBR::AirbrakeConstants;

inline float celsius_to_kelvin(float temp)
{
    return temp + 273.15;
}

float calc_altitude(float init_temp, float init_press, float curr_press)
{
    // Given celsius so convert to kelvin
    init_temp = celsius_to_kelvin(init_temp);
    float exp = kUnivGasConst * kTempLapseRate / kGravity / kAirMolarMass;
    return init_temp / kTempLapseRate *
           (1 - std::pow(curr_press / init_press, exp));
}

float calc_velocity(float v0, float aZ, float time)
{
    return v0 + aZ * time;
}

bool monitor_imu(Bno055& bno055)
{
    // I just copied paste from the imu test
    uint8_t chip_id = 0;
    Bno055::Mode opr_mode = Bno055::Mode::CONFIG;
    uint8_t sys_status = 0;
    uint8_t self_test = 0;
    uint8_t sys_error = 0;

    return bno055.get_chip_id(chip_id) && bno055.get_opr_mode(opr_mode) &&
           bno055.get_sys_status(sys_status) && bno055.run_post(self_test) &&
           bno055.get_sys_error(sys_error);
}

bool monitor_baro(Bmp390& bmp390)
{
    return true;
}