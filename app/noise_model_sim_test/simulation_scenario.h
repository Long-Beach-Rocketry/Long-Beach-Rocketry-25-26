/**
 * @file simulation_scenario.h
 * @brief generate ideal barometer inputs for a scripted simulation
 */

#pragma once

#include "simulation_config.h"

namespace LBR
{

/**
 * @brief hold ideal pressure in pa and temperature in degrees celsius
 *
 * these values remain separate from the noisy measurements
 */
struct BarometerTruth
{
    double pressure_pa;
    double temperature_c;
};

/**
 * @brief return the ideal barometer readings at a given simulation time
 *
 * apply a scripted straight-line change, not a validated flight model
 * the same time and configuration always produce the same ideal values
 *
 * @param config initial values and rates of change for each channel
 * @param time_s absolute seconds since simulation start
 * @return ideal pressure in pa and temperature in degrees celsius
 */
inline BarometerTruth barometer_truth_at(const NoiseSimulationConfig& config,
                                         double time_s)
{
    return {
        config.initial_pressure_pa + config.pressure_change_pa_per_s * time_s,
        config.initial_temperature_c +
            config.temperature_change_c_per_s * time_s,
    };
}

}  // namespace LBR
