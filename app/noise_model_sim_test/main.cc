/**
 * @file main.cc
 * @brief Print ideal and noisy barometer readings over simulated time.
 *
 * Run commands from the repository root.
 *
 * Linux terminal (also works inside the dev container):
 *   cmake --preset native -B build/noise-demo-linux
 *   cmake --build build/noise-demo-linux --target noise_model_sim
 *   ./build/noise-demo-linux/app/noise_model_sim_test/noise_model_sim
 *
 * Windows PowerShell through Ubuntu WSL :
 *   wsl -d Ubuntu -- cmake --preset native -B build/noise-demo-wsl
 *   wsl -d Ubuntu -- cmake --build build/noise-demo-wsl --target noise_model_sim
 *   wsl -d Ubuntu -- ./build/noise-demo-wsl/app/noise_model_sim_test/noise_model_sim
 * Replace Ubuntu with your WSL distribution name if it differs.
 * Use separate build directories for WSL, containers, and other environments.
 * To use the SITL configuration, change the preset from native to sitl.
 *
 * units are pa, degrees celsius, and simulated seconds
 *
 * the default inputs change linearly and are purely illustrative!
 */

#include <cmath>
#include <iomanip>
#include <iostream>

#include "noise_model.h"
#include "simulation_config.h"
#include "simulation_scenario.h"

using namespace std;

int main()
{
    // load read-only settings without requiring compile-time evaluation
    const SimulationConfig config{};

    // validate timeline settings before generating any readings
    if (!isfinite(config.time_step_s) || config.time_step_s <= 0.0)
    {
        cerr << "Time step must be finite and positive\n";
        return 1;
    }
    if (config.sample_intervals < 0)
    {
        cerr << "Interval count must be nonnegative\n";
        return 1;
    }

    LBR::NoiseModel pressure_noise(config.pressure_noise, config.pressure_seed); // pressure test
    LBR::NoiseModel temperature_noise(config.temperature_noise, config.temperature_seed); // temp test

    cout << "Ideal -> simulated barometer readings\n";
    cout << fixed << setprecision(4);

    // START OF LOOP: start at zero, include the final interval, and advance one step at a time.
    for (int step = 0; step <= config.sample_intervals; ++step)
    {
        // time since simulation start: 0.0, 0.1, ... 10.0 seconds
        const double time_s = step * config.time_step_s;

        // obtain this instant's ideal values before adding measurement errors
        const BarometerTruth truth = barometer_truth_at(config, time_s);

        // sample() adds bias, elapsed-time drift, fresh jitter, then rounding.
        const double noisy_pressure_pa = pressure_noise.sample(truth.pressure_pa, time_s);
        const double noisy_temperature_c = temperature_noise.sample(truth.temperature_c, time_s);

        cout << "Time: " << time_s << " s\n"
             << "  Pressure (Pa):   true = " << setw(8) << truth.pressure_pa
             << "   noisy = " << setw(8) << noisy_pressure_pa << '\n'
             << "  Temperature (C): true = " << setw(8) << truth.temperature_c
             << "   noisy = " << setw(8) << noisy_temperature_c << "\n\n";
    }

    return 0;
}
// https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp390-ds002.pdf noise and temp jitter setting
