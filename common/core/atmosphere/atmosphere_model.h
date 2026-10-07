/* 
* @file atmosphere_model.h
* @author Mario Cruz
* @brief Data structure and function declared for Atmosphere Model specific to SITL pipeline
* @date 9-30-2026
*/

#ifndef ATMOSPHERE_MODEL_H
#define ATMOSPHERE_MODEL_H

#include "atmosphere.h"

// True (noise-free) pressure and temperature at a given altitude
typedef struct
{
    double pressure_pa;
    double temperature_k;
} BarometerRawValues;  // BTW not literal data pulled from a sensor

#ifdef __cplusplus
extern "C"
{
#endif

    // Returns barometer readings for a given altitude
    BarometerRawValues compute_barometer_raw_values(
        double true_altitude_above_ground_m, const GroundConditions* ground);

#ifdef __cplusplus
}
#endif
#endif