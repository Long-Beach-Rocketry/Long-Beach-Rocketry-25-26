#pragma once

#include <cstdint>

#include "noise_model.h"

struct SimulationConfig
{
    // seconds between readings and number of intervals
    // including time zero produces one more reading than the interval count
    double time_step_s = 0.1;
    int sample_intervals = 100;

    // scripted pressure starts here and changes by this many pa each second
    double initial_pressure_pa = 90000.0;
    double pressure_change_pa_per_s = -100.0;

    // scripted temperature starts here and changes by this many celsius per second
    double initial_temperature_c = 20.0;
    double temperature_change_c_per_s = -0.05;

    // separate error settings for each measurement channel
    LBR::NoiseModelConfig pressure_noise{};
    LBR::NoiseModelConfig temperature_noise{};

    // distinct seeds select repeatable starting sequences for the two channels
    uint32_t pressure_seed = 123U;
    uint32_t temperature_seed = 456U;

    // this constructor applies named defaults when a configuration is created
    SimulationConfig()
    {
        // pressure errors use pa, with drift in pa per second
        // a spread of 2 pa can also produce errors beyond plus or minus 2 pa
        pressure_noise.jitter_stddev = 2.0;
        pressure_noise.fixed_bias = 0.0;
        pressure_noise.bias_drift_per_s = 0.0;
        pressure_noise.quantization_step = 0.0;  // zero disables rounding

        // temperature errors use degrees celsius, with drift per second
        // set all four fields to zero to observe ideal measurements alone
        temperature_noise.jitter_stddev = 0.005;
        temperature_noise.fixed_bias = 0.0;
        temperature_noise.bias_drift_per_s = 0.0;
        temperature_noise.quantization_step = 0.0;
    }
};
