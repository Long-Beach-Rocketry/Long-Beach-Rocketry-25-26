/// @file noise_model.h
/// @brief add measurement errors to simulated sensor values

#pragma once

#include <cstdint>
#include <random>

namespace LBR
{

// settings for one channel, such as pressure or temperature
// use channel units for offsets and rounding, and units per second for drift
struct NoiseModelConfig
{
    double jitter_stddev = 0.0;     // random spread, not an error limit
    double fixed_bias = 0.0;        // constant offset added to every reading
    double bias_drift_per_s = 0.0;  // signed offset change per simulated second
    // rounding step in channel units, with zero disabling rounding
    double quantization_step = 0.0;
};

// keep one model per channel so its random sequence persists between readings
// use distinct channel seeds to avoid starting with the same random sequence
class NoiseModel
{
public:
    // copy the settings and seed the generator once when this object is created
    // the same seed and calls repeat a sequence on the same toolchain
    explicit NoiseModel(NoiseModelConfig config, uint32_t seed = 1U);

    /// @param true_value ideal measurement from the physics model
    /// @param time_s absolute seconds since simulation start, not a time step

    // return the reading after bias, drift, fresh jitter, and optional rounding
    // nodiscard asks the compiler to warn if the caller ignores this reading
    [[nodiscard]] double sample(double true_value, double time_s);

private:
    NoiseModelConfig config_;  // copied settings, unaffected by caller edits
    std::minstd_rand generator_;  // persistent random state
    // convert the generator's output into jitter with zero mean and unit spread
    std::normal_distribution<double> standard_normal_;
};

}  // project namespace
