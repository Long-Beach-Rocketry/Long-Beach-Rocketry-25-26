/**
 * @file main.cc
 * @brief print ideal and noisy barometer readings over simulated time
 *
 * see README.md for build commands, configuration, units, and model limitations
 */

#include <cmath>
#include <iomanip>
#include <iostream>

#include "noise_model.h"
#include "simulation_config.h"
#include "simulation_scenario.h"

int main()
{
    // load read-only settings without requiring compile-time evaluation
    const LBR::NoiseSimulationConfig config{};

    // validate timeline settings before generating any readings
    if (!std::isfinite(config.time_step_s) || config.time_step_s <= 0.0)
    {
        std::cerr << "Time step must be finite and positive\n";
        return 1;
    }
    if (config.sample_intervals < 0)
    {
        std::cerr << "Interval count must be nonnegative\n";
        return 1;
    }

    LBR::NoiseModel pressure_noise(config.pressure_noise,
                                   config.pressure_seed);  // pressure test
    LBR::NoiseModel temperature_noise(config.temperature_noise,
                                      config.temperature_seed);  // temp test

    std::cout << "Ideal -> simulated barometer readings\n";
    std::cout << std::fixed << std::setprecision(4);

    // LOOP: start at zero, include the final interval, and advance one step at a time.
    for (int step = 0; step <= config.sample_intervals; ++step)
    {
        // time since simulation start: 0.0, 0.1, ... 10.0 seconds
        const double time_s = step * config.time_step_s;

        // obtain this instant's ideal values before adding measurement errors
        const LBR::BarometerTruth truth =
            LBR::barometer_truth_at(config, time_s);

        // sample() adds bias, elapsed-time drift, fresh jitter, then rounding.
        const double noisy_pressure_pa =
            pressure_noise.sample(truth.pressure_pa, time_s);
        const double noisy_temperature_c =
            temperature_noise.sample(truth.temperature_c, time_s);

        std::cout << "Time: " << time_s << " s\n"
                  << "  Pressure (Pa):   true = " << std::setw(8)
                  << truth.pressure_pa << "   noisy = " << std::setw(8)
                  << noisy_pressure_pa << '\n'
                  << "  Temperature (C): true = " << std::setw(8)
                  << truth.temperature_c << "   noisy = " << std::setw(8)
                  << noisy_temperature_c << "\n\n";
    }

    return 0;
}
