#include <cstdio>
#include <cstdlib>
#include "atmosphere.h"

using namespace LBR;

// Unit conversions to compare against the U.S. Standard Atmosphere table
constexpr double FeetToMeters = 0.3048;
constexpr double KgPerM3ToSlugsPerFt3 = 0.00194032;
constexpr double PascalsToPsi = 1.0 / 6894.757;

double kelvin_to_fahrenheit(double kelvin)
{
    return (kelvin - 273.15) * 9.0 / 5.0 + 32.0;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::printf("usage: %s <elevation_ft>\n", argv[0]);
        return 1;
    }

    // Input is elevation in feet above sea level, converted to meters
    const double elevation_ft = std::atof(argv[1]);
    const double elevation_m = elevation_ft * FeetToMeters;

    // Standard sea level as the "launch pad", so altitude = elevation
    AtmosphereConditions sea_level{};
    Atmosphere::standard_atmosphere(0.0, sea_level);
    const Atmosphere atmosphere{sea_level};

    // Pressure and temperature from the model, density from them
    AtmosphereConditions conditions{};
    double density_kgm3 = 0.0;
    if (!atmosphere.sim_barometer(elevation_m, conditions) ||
        !atmosphere.density_kgm3(elevation_m, density_kgm3))
    {
        std::printf("calculation failed at %.0f ft\n", elevation_ft);
        return 1;
    }

    std::printf("elevation:   %.0f ft\n", elevation_ft);
    std::printf("temperature: %.2f F\n",
                kelvin_to_fahrenheit(conditions.temperature_k));
    std::printf("pressure:    %.3f psi\n", conditions.pressure_pa * PascalsToPsi);
    std::printf("density:     %.2f x10^-4 slugs/ft^3\n",
                density_kgm3 * KgPerM3ToSlugsPerFt3 * 1e4);
    std::printf("density:     %.4f kg/m^3\n", density_kgm3);
    return 0;
}

// cd common/core/atmosphere
// g++ -std=c++20 density_check.cc atmosphere.cc -o density_check
// ./density_check 0
// ./density_check 5000
// ./density_check 6561.68