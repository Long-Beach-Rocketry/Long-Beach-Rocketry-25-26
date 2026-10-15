/* 
* @file atmosphere_model.cc
* @author Mario Cruz
* @brief Implements the Atmosphere Model's barometer output calculation specific to SITL pipeline
* @date 9-30-2026
*/

#include "atmosphere_model.h"

// Combines pressure and temperature into one barometer reading
BarometerRawValues compute_barometer_raw_values(
    double true_altitude_above_ground_m, const GroundConditions* ground)
{
    BarometerRawValues values;
    values.pressure_pa =
        atmosphere_pressure_pa(true_altitude_above_ground_m, ground);
    values.temperature_k =
        atmosphere_temperature_k(true_altitude_above_ground_m, ground);
    return values;
}