#pragma once

#include "simulation_config.h"

// ideal sensor values remain separate from the noisy measurements
struct BarometerTruth
{
    double pressure_pa;
    double temperature_c;
};

// a scripted linear change tests moving inputs without claiming flight physics
// the same time and configuration always produce the same ideal values
inline BarometerTruth barometer_truth_at(const SimulationConfig& config,
                                         double time_s)
{
    return {
        config.initial_pressure_pa + config.pressure_change_pa_per_s * time_s,
        config.initial_temperature_c +
            config.temperature_change_c_per_s * time_s,
    };
}
