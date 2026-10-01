/* 
* @file atmosphere.h
* @author Mario Cruz
* @brief Data structure and function declared for Atmosphere Model specific to SITL pipeline
* @date 9-30-2026
*/

#ifndef ATMOSPHERE_MODEL_H
#define ATMOSPHERE_MODEL_H

#include "atmosphere.h"

// True pressure and temperature at given altitude
typedef struct {
    double pressure_pa;
    double temperature_k;
} BarometerRawValues;       // BTW not literal sensor data

#ifdef __cplusplus
extern "C" {
#endif

// Returns barometer readings for a given altitude
BarometerRawValues compute_barometer_raw_values(
    double true_altitude_above_ground_m,
    const GroundConditions* ground
);

#ifdef __cplusplus
}
#endif
#endif