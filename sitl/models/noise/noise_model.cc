#include "noise_model.h"

#include <cmath>
#include <stdexcept>

using namespace std;

namespace LBR
{

NoiseModel::NoiseModel(NoiseModelConfig config, uint32_t seed)
    : config_(config), generator_(seed), standard_normal_(0.0, 1.0)
{
    // Validate that all noise parameters are finite and that jitter and
    // quantization resolution are nonnegative before accepting the config.
    if (!isfinite(config_.jitter_stddev) || !isfinite(config_.fixed_bias) ||
        !isfinite(config_.bias_drift_per_s) ||
        !isfinite(config_.quantization_step) ||
        config_.jitter_stddev < 0.0 || config_.quantization_step < 0.0)
    {
        throw invalid_argument(
            "NoiseModel requires finite parameters "
            "and nonnegative jitter/resolution");
    }
}

double NoiseModel::sample(double true_value, double time_s)
{
    if (!isfinite(true_value) || !isfinite(time_s) || time_s < 0.0)
    {
        throw invalid_argument(
            "NoiseModel requires a finite true value "
            "and nonnegative simulation time");
    }

    // Absolute simulation time keeps drift independent of sensor sample rate.
    // A second sample at the same instant therefore has the same drift bias.
    const double drifting_bias = config_.bias_drift_per_s * time_s;
    double reading = true_value + config_.fixed_bias + drifting_bias;

    if (config_.jitter_stddev > 0.0)
    {
        // Fresh zero-mean Gaussian jitter models reading-to-reading variation.
        reading += config_.jitter_stddev * standard_normal_(generator_);
    }

    if (!isfinite(reading))
    {
        throw overflow_error("NoiseModel reading exceeded finite range");
    }

    if (config_.quantization_step > 0.0)
    {
        // Quantize last, as a physical sensor rounds its already imperfect
        // analog measurement to the nearest representable step.
        reading = round(reading / config_.quantization_step) *
                  config_.quantization_step;
    }

    return reading;
}

}  // namespace LBR
