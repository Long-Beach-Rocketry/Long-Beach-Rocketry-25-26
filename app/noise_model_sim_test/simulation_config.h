/**
 * @file simulation_config.h
 * @brief define editable settings for the barometer noise demonstration
 */

#pragma once

#include <cstdint>

#include "noise_model.h"

namespace LBR
{

/**
 * @brief group timeline, scripted environment, noise settings, and channel seeds
 *
 * edit these defaults, then rebuild and rerun as described in README.md
 * environment changes are separate from errors added to sensor readings
 */
struct NoiseSimulationConfig
{
    // seconds between readings and number of intervals
    // including time zero produces one more reading than the interval count

    double time_step_s = 0.1;    // Change steps per second here
    int sample_intervals = 100;  // Change how many steps total here

    // scripted pressure starts here and changes by this many pa each second
    double initial_pressure_pa = 90000.0;      // Change initial pressure here
    double pressure_change_pa_per_s = -100.0;  // Change PA/s rate here

    // scripted temperature starts here and changes by this many celsius per second
    double initial_temperature_c = 20.0;        // Change initial temps here
    double temperature_change_c_per_s = -0.05;  // Change Celsius/s rate here

    // separate error settings for each measurement channel
    NoiseModelConfig pressure_noise{};
    NoiseModelConfig temperature_noise{};

    // distinct seeds select repeatable starting sequences for the two channels
    std::uint32_t pressure_seed = 123U;
    std::uint32_t temperature_seed = 456U;

    /**
     * @brief initialize the pressure and temperature noise settings
     *
     * jitter starts at the documented baseline with other errors disabled
     */

    NoiseSimulationConfig()
    {
        // set all four fields to zero to observe ideal measurements alone

        // pressure errors use pa, with drift in pa per second
        // a jitter_stddev of 2 can also produce errors beyond plus or minus 2

        pressure_noise.jitter_stddev =
            2.0;  // Change this line to alter the random pressure noise spread (Pa).
        pressure_noise.fixed_bias =
            0.0;  // Change this line to add a constant pressure offset (Pa).
        pressure_noise.bias_drift_per_s =
            0.0;  // Change this line to add a pressure offset drift over time (Pa/s).
        pressure_noise.quantization_step =
            0.0;  // Change this line to set pressure rounding size (Pa); zero disables rounding.

        temperature_noise.jitter_stddev =
            0.005;  // Change this line to alter the random temperature noise spread (C).
        temperature_noise.fixed_bias =
            0.0;  // Change this line to add a constant temperature offset (C).
        temperature_noise.bias_drift_per_s =
            0.0;  // Change this line to add a temperature offset drift over time (C/s).
        temperature_noise.quantization_step =
            0.0;  // Change this line to set temperature rounding size (C); zero disables rounding.
    }
};

}  // namespace LBR
