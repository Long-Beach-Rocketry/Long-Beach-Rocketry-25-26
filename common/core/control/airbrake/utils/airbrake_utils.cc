#include <cmath>
#include "airbrake_constants.h"

using namespace LBR::AirbrakeConstants;

inline float celsiusToKelvin(float temp)
{
    return temp + 273.15;
}

float calcAltitude(float init_temp, float init_press, float curr_press)
{
    // Given celsius so convert to kelvin
    init_temp = celsiusToKelvin(init_temp);
    float exp = kUnivGasConst * kTempLapseRate / kGravity / kAirMolarMass;
    return init_temp / kTempLapseRate *
           (1 - std::pow(curr_press / init_press, exp));
}

float calcVelocity(float v0, float aZ, float time)
{
    return v0 + aZ * time;
}
