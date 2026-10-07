/* 
* @file atmosphere.h
* @author Mario Cruz
* @brief Generic ISA/barometric physics (not specific to SITL),
*        reusable for future applications needing atmospheric physics
* @date 9-30-2026
*/

#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

// Ground-level temperature and pressure measured at the launch pad
// Used as the starting point for all altitude calculations below
typedef struct
{
    double temperature_k;
    double pressure_pa;
} GroundConditions;

// Standard sea-level reference conditions, used only as placeholders
// While real ground conditions haven't been measured yet
#define ISA_SEA_LEVEL_TEMPERATURE_K 288.15  // 15°C, standard day
#define ISA_SEA_LEVEL_PRESSURE_PA 101325.0  // 1 atmosphere

#ifdef __cplusplus
extern "C"
{
#endif

    // Returns temperature at given height above ground level
    double atmosphere_temperature_k(double height_above_ground_m,
                                    const GroundConditions* ground);

    // Returns pressure at given height above ground level
    double atmosphere_pressure_pa(double height_above_ground_m,
                                  const GroundConditions* ground);

    // Returns air density at a given height above ground level
    double atmosphere_density_kgm3(double height_above_ground_m,
                                   const GroundConditions* ground);

    // Returns ground conditions for a given site elevation
    // Placeholder until real measured conditions are available
    GroundConditions ground_conditions_for_elevation_m(double elevation_m);

#ifdef __cplusplus
}
#endif
#endif