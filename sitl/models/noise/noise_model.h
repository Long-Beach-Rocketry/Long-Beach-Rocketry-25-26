/**
 * @file noise_model.h
 * @brief add measurement errors to simulated sensor values
 */

#pragma once

#include <cstdint>
#include <random>

namespace LBR
{

/**
 * @brief configure measurement errors for one sensor channel
 *
 * use channel units for jitter, bias, and rounding
 * use channel units per second for drift
 * zero-valued settings disable the corresponding error
 */
struct NoiseModelConfig
{
    double jitter_stddev = 0.0;     // random spread, not an error limit
    double fixed_bias = 0.0;        // constant offset added to every reading
    double bias_drift_per_s = 0.0;  // signed offset change per simulated second
    // rounding step in channel units, with zero disabling rounding
    double quantization_step = 0.0;
};

/**
 * @brief convert ideal scalar measurements into simulated sensor readings
 *
 * keep one model per channel so its random sequence persists between readings
 * use distinct channel seeds to avoid starting with the same random sequence
 */
class NoiseModel
{
public:
    /**
     * @brief copy the channel settings and seed its random generator
     * @param config error settings in this channel's units
     * @param seed starting seed for repeatable runs on the same toolchain
     * @throws std::invalid_argument if settings are nonfinite, or jitter or
     *         rounding step is negative
     */
    explicit NoiseModel(NoiseModelConfig config, std::uint32_t seed = 1U);

    /**
     * @brief add bias, time-based drift, fresh jitter, and optional step rounding
     * @param true_value ideal measurement in this channel's units
     * @param time_s absolute seconds since simulation start, not a time step
     * @return the simulated sensor reading in the same units as true_value
     * @throws std::invalid_argument if inputs are nonfinite or time is negative
     * @throws std::overflow_error if the combined reading is nonfinite
     *         before rounding
     */
    [[nodiscard]] double sample(double true_value, double time_s);

private:
    NoiseModelConfig config_;  // copied settings, unaffected by caller edits
    std::mt19937 generator_;   // persistent mersenne twister state
    // convert the generator's output into jitter with zero mean and unit spread
    std::normal_distribution<double> standard_normal_;
};

}  // namespace LBR
