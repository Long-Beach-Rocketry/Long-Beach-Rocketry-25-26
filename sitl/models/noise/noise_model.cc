#include "noise_model.h"

#include <cmath>
#include <stdexcept>

namespace LBR
{

// initialize owned settings, the seeded generator, and a standard gaussian
// the gaussian starts with mean zero and standard deviation one
NoiseModel::NoiseModel(NoiseModelConfig config, std::uint32_t seed)
    : config_(config), generator_(seed), standard_normal_(0.0, 1.0)
{
    // reject infinities, invalid numbers, and negative jitter or rounding steps
    // signed bias and drift remain valid because errors can point either way
    if (!std::isfinite(config_.jitter_stddev) ||
        !std::isfinite(config_.fixed_bias) ||
        !std::isfinite(config_.bias_drift_per_s) ||
        !std::isfinite(config_.quantization_step) ||
        config_.jitter_stddev < 0.0 || config_.quantization_step < 0.0)
    {
        throw std::invalid_argument(
            "NoiseModel requires finite parameters "
            "and nonnegative jitter/resolution");
    }
}

double NoiseModel::sample(double true_value, double time_s)
{
    // reject invalid measurements and times before doing arithmetic
    if (!std::isfinite(true_value) || !std::isfinite(time_s) || time_s < 0.0)
    {
        throw std::invalid_argument(
            "NoiseModel requires a finite true value "
            "and nonnegative simulation time");
    }

    // absolute time makes drift independent of how often this method is called
    // the same time keeps the same drift, but each call can draw new jitter
    const double drifting_bias = config_.bias_drift_per_s * time_s;
    double reading = true_value + config_.fixed_bias + drifting_bias;

    if (config_.jitter_stddev > 0.0)
    {
        // draw fresh jitter and scale its spread into this channel's units
        // zero skips the draw, while positive spread is not an error limit
        reading += config_.jitter_stddev * standard_normal_(generator_);
    }

    // finite inputs can still overflow when their errors are combined
    if (!std::isfinite(reading))
    {
        throw std::overflow_error("NoiseModel reading exceeded finite range");
    }

    if (config_.quantization_step > 0.0)
    {
        // round the imperfect measurement to the nearest configured step last
        // divide to count steps, round the count, then restore channel units
        reading = std::round(reading / config_.quantization_step) *
                  config_.quantization_step;
    }

    return reading;
}

}  // namespace LBR
