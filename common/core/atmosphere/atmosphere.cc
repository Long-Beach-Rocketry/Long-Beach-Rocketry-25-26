/* 
* @file atmosphere.cc
* @author Mario Cruz
* @brief Implements generic ISA/barometric physics equations declared in atmosphere.h (not specific to SITL)
* @date 9-30-2026
*/

// Implements the functions declared in "atmosphere.h"
#include "atmosphere.h"
#include <math.h>

// Standard atmospheric physics constants
#define TROPOSPHERE_LAPSE_RATE_K_PER_M 0.0065
#define STANDARD_GRAVITY_MPS2 9.80665
#define MOLAR_MASS_DRY_AIR_KG_PER_MOL 0.0289644
#define UNIVERSAL_GAS_CONSTANT_J_PER_MOL_K 8.3144598
#define SPECIFIC_GAS_CONSTANT_DRY_AIR 287.05

// atmosphere temperature = ground temperature - (lapse rate * height)
double atmosphere_temperature_k(double height_above_ground_m,
                                const GroundConditions* ground)
{
    return ground->temperature_k -
           TROPOSPHERE_LAPSE_RATE_K_PER_M * height_above_ground_m;
}

// atmosphere pressure = ground pressure * barometric formula factor
double atmosphere_pressure_pa(double height_above_ground_m,
                              const GroundConditions* ground)
{
    const double exponent =
        (STANDARD_GRAVITY_MPS2 * MOLAR_MASS_DRY_AIR_KG_PER_MOL) /
        (UNIVERSAL_GAS_CONSTANT_J_PER_MOL_K * TROPOSPHERE_LAPSE_RATE_K_PER_M);
    const double base_ratio =
        1.0 - (TROPOSPHERE_LAPSE_RATE_K_PER_M * height_above_ground_m) /
                  ground->temperature_k;
    return ground->pressure_pa * pow(base_ratio, exponent);
}

// atmosphere density = pressure / (specific gas constant * temperature)
// THE number that ends up in the rocket's drag equation
double atmosphere_density_kgm3(double height_above_ground_m,
                               const GroundConditions* ground)
{
    const double temperature_k =
        atmosphere_temperature_k(height_above_ground_m, ground);
    const double pressure_pa =
        atmosphere_pressure_pa(height_above_ground_m, ground);
    return pressure_pa / (SPECIFIC_GAS_CONSTANT_DRY_AIR * temperature_k);
}

// Returns standard ground conditions at a given elevation
// Calculated as if sea level were the ground and elevation_m were the height above it
GroundConditions ground_conditions_for_elevation_m(double elevation_m)
{
    GroundConditions sea_level_ground = {ISA_SEA_LEVEL_TEMPERATURE_K,
                                         ISA_SEA_LEVEL_PRESSURE_PA};
    GroundConditions ground;
    ground.temperature_k =
        atmosphere_temperature_k(elevation_m, &sea_level_ground);
    ground.pressure_pa = atmosphere_pressure_pa(elevation_m, &sea_level_ground);
    return ground;
}