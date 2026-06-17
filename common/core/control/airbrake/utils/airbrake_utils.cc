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

bool monitor_imu(void)
{
    return true;
}

bool monitor_baro(void)
{
    return true;
}