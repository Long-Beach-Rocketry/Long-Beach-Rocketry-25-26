/**
 * @file noise_model.h
 * @brief Scalar sensor errors for software-in-the-loop simulations.
 */

#pragma once

#include <cstdint>
#include <random>

namespace LBR
{

/**
 * @brief Parameters for one sensor channel, all in that channel's units.
 *
 * For example, a pressure model uses pascals and pascals per second; a
 * temperature model uses degrees Celsius and degrees Celsius per second.
 */
struct NoiseModelConfig
{
    double jitter_stddev = 0.0;     // Standard deviation of each new reading.
    double fixed_bias = 0.0;        // Offset that remains constant for a run.
    double bias_drift_per_s = 0.0;  // Signed change in offset per simulated s.
    // Sensor resolution in channel units; zero disables rounding.
    double quantization_step = 0.0;
};

/**
 * @brief Converts a simulator's true scalar value into a sensor-like reading.
 *
 * Keep one instance per measured channel so each channel has its own random
 * sequence. The default seed repeats a run on the same toolchain; callers can
 * choose another seed to exercise a different jitter sequence.
 */
class NoiseModel
{
public:
    explicit NoiseModel(NoiseModelConfig config, uint32_t seed = 1U);

    /**
     * @param true_value Noise-free value supplied by the physics model.
     * @param time_s Seconds since simulation start, not time since last call.
     * @return Value after bias, drift, jitter, then sensor-step rounding.
     */
    [[nodiscard]] double sample(double true_value, double time_s);

private:
    NoiseModelConfig config_;
    std::minstd_rand generator_;
    std::normal_distribution<double> standard_normal_;
};

}  // namespace LBR
