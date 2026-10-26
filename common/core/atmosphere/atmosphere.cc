/* 
* @file atmosphere.cc
* @author Mario Cruz
* @brief Implements the ISA troposphere equations declared in atmosphere.h
* @date 9-30-2026
*/

#include "atmosphere.h"
#include <cmath>

namespace LBR
{

namespace
{
// --------------------------------------------------------------------------------
// Constants - All values are pulled from the 1976 U.S Standard Atmosphere book
// {https://ntrs.nasa.gov/api/citations/19770003812/downloads/19770003812.pdf}
// --------------------------------------------------------------------------------

// Standard day air conditions at sea level, used in standard_atmosphere() for SITL, tests, or backup data
constexpr double SeaLevelPressurePa{101325.0};  // 1 atmosphere
constexpr double SeaLevelTemperatureK{288.15};  // 15 C

constexpr double LapseRateKPerM{
    0.0065};  // Troposphere lapse rate, air gets 0.0065K colder per meter climbed

constexpr double GravityMps2{9.80665};             // Standard gravity
constexpr double MolarMassAirKgPerMol{0.0289644};  // Mean molar mass of dry air
constexpr double UniversalGasConstant{
    8.31432};  // Universal gas constant, J/(mol*K)
constexpr double SpecificGasConstantAir{
    UniversalGasConstant /
    MolarMassAirKgPerMol};  // Gas constant for dry air per kilogram, J/(kg*K)

// Sets how steeply pressure falls as the air cools, barometric formula: g * M / (R * L) = 5.2558
constexpr double PressureExponent{(GravityMps2 * MolarMassAirKgPerMol) /
                                  (UniversalGasConstant * LapseRateKPerM)};

// --------------------------------------------------------------------------------
// Safety checks
// --------------------------------------------------------------------------------

// Pressure, temperature, and density are real & positive numbers
bool positive_and_finite(double value)
{
    return std::isfinite(value) && value > 0.0;
}

// Check barometer reading conditions are real & positive
bool launch_conditions_valid(const AtmosphereConditions& launch_conditions)
{
    return positive_and_finite(launch_conditions.pressure_pa) &&
           positive_and_finite(launch_conditions.temperature_k);
}

// --------------------------------------------------------------------------------
// Troposphere equations
// --------------------------------------------------------------------------------

// Rule 1, temperature drops in a straight line: T = T0 - L * h
double calc_temperature_k(const AtmosphereConditions& launch_conditions,
                          double altitude_m)
{
    return launch_conditions.temperature_k - LapseRateKPerM * altitude_m;
}

// Rule 2, barometric formula: P = P0 * (T / T0) ^ 5.25588
double calc_pressure_pa(const AtmosphereConditions& launch_conditions,
                        double altitude_m)
{
    const double temperature =
        calc_temperature_k(launch_conditions, altitude_m) /
        launch_conditions.temperature_k;
    return launch_conditions.pressure_pa *
           std::pow(temperature, PressureExponent);
}

// Run both rules 1 and 1, checks the inputs before and results after
bool calc_conditions(const AtmosphereConditions& launch_conditions,
                     double altitude_m, AtmosphereConditions& conditions_out)
{
    // Check altitude and atmosphere conditions are valid numbers
    if (!std::isfinite(altitude_m) ||
        !launch_conditions_valid(launch_conditions))
    {
        return false;
    }

    const AtmosphereConditions conditions{
        .pressure_pa = calc_pressure_pa(launch_conditions, altitude_m),
        .temperature_k = calc_temperature_k(launch_conditions, altitude_m)};

    if (!positive_and_finite(conditions.pressure_pa) ||
        !positive_and_finite(conditions.temperature_k))
    {
        return false;
    }
    conditions_out = conditions;
    return true;
}

}  // namespace

// --------------------------------------------------------------------------------
// Atmosphere class
// --------------------------------------------------------------------------------

Atmosphere::Atmosphere(const AtmosphereConditions& launch_conditions_)
    : launch_conditions{launch_conditions_}
{
}

// Standard conditions at a launch site, used as launch conditions until real ones are measured
bool Atmosphere::standard_atmosphere(
    double elevation_m, AtmosphereConditions& launch_conditions_out)
{
    const AtmosphereConditions sea_level{.pressure_pa = SeaLevelPressurePa,
                                         .temperature_k = SeaLevelTemperatureK};
    return calc_conditions(sea_level, elevation_m, launch_conditions_out);
}

// Safety checks that launch conditions are physically possible
bool Atmosphere::is_valid() const
{
    return launch_conditions_valid(launch_conditions);
}

// Rule 3, ideal gas law: density = P / (Rs * T)
bool Atmosphere::density_kgm3(double altitude_m, double& density_out) const
{
    AtmosphereConditions conditions{};
    if (!calc_conditions(launch_conditions, altitude_m, conditions))
    {
        return false;
    }
    density_out = conditions.pressure_pa /
                  (SpecificGasConstantAir * conditions.temperature_k);
    return true;
}

// Simulated barometer for SITL testing, gives pressure and temperature
bool Atmosphere::sim_barometer(double altitude_m,
                               AtmosphereConditions& conditions_out) const
{
    return calc_conditions(launch_conditions, altitude_m, conditions_out);
}

}  // namespace LBR