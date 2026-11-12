# Barometer noise demonstration

main.cc prints ideal and noisy pressure/temperature readings over
time. Its scripted inputs change smoothly so developers can observe
measurement errors while the underlying values change. It is a standalone
demonstration, not supposed to mimic real flight.

## Files

| File | Purpose |
| --- | --- |
| [main.cc](main.cc) | Validate timing, run the loop, and print labeled readings |
| [simulation_config.h](simulation_config.h) | Editable timing, environment, noise settings, and seeds |
| [simulation_scenario.h](simulation_scenario.h) | Calculate ideal values from the initial values and rates |
| [CMakeLists.txt](CMakeLists.txt) | Build the `noise_model_sim` executable for native/SITL |

The executable links the shared [noise-model library](../../sitl/models/noise/README.md).
The folder is named `noise_model_sim_test`; the executable is `noise_model_sim`.

## Requirements

Use a Linux terminal, Ubuntu WSL, or the project's Docker dev container with
CMake 3.26+, Ninja, and a C++20 compiler. Initialize dependencies once for a
fresh checkout:

```sh
git submodule update --init --recursive
```

Run all commands from the repository root, not from this application folder.
Docker is optional when using WSL. No sensor hardware is required.

## Build and run

### Linux or dev-container terminal

```sh
cmake --preset native -B build/noise-demo-linux
cmake --build build/noise-demo-linux --target noise_model_sim
./build/noise-demo-linux/app/noise_model_sim_test/noise_model_sim
```

### Windows PowerShell through Ubuntu WSL

Open PowerShell at the repository root and run:

```powershell
wsl -d Ubuntu -- cmake --preset native -B build/noise-demo-wsl
wsl -d Ubuntu -- cmake --build build/noise-demo-wsl --target noise_model_sim
wsl -d Ubuntu -- ./build/noise-demo-wsl/app/noise_model_sim_test/noise_model_sim
```

Replace `Ubuntu` with your installed distribution name if necessary. These
commands build and run Linux programs through WSL; the C++ compiler used by
Windows VS Code IntelliSense is a separate editor setting.

### SITL configuration

From a Linux or dev-container terminal:

```sh
cmake --preset sitl -B build/noise-demo-sitl
cmake --build build/noise-demo-sitl --target noise_model_sim
./build/noise-demo-sitl/app/noise_model_sim_test/noise_model_sim
```

For PowerShell, prefix each command with `wsl -d Ubuntu --`. Both presets run
this same application on the development computer. Use separate build folders
for WSL, Docker, and other environments because cached source/compiler paths
can differ. [CMake documents these preset and build options](https://cmake.org/cmake/help/latest/manual/cmake.1.html).

## Configure a simulation

Edit [simulation_config.h](simulation_config.h), save it, rebuild the target,
and run again. There are no interactive prompts or command-line configuration
options. Timeline and ideal-input defaults are member initializers; noise
settings are named assignments inside the `NoiseSimulationConfig()` constructor.

| Setting | Default | Meaning |
| --- | --- | --- |
| `time_step_s` | `0.1` | Simulated seconds between readings |
| `sample_intervals` | `100` | Intervals; including time zero gives 101 readings |
| `initial_pressure_pa` | `90000.0` | Initial ideal pressure in Pa |
| `pressure_change_pa_per_s` | `-100.0` | Ideal pressure change in Pa/s |
| `initial_temperature_c` | `20.0` | Initial ideal temperature in degrees Celsius |
| `temperature_change_c_per_s` | `-0.05` | Ideal temperature change in degrees Celsius/s |
| `pressure_seed` / `temperature_seed` | `123U` / `456U` | Starting random sequences for the channels |
| `pressure_noise.jitter_stddev` | `2.0` | Pressure jitter standard deviation in Pa |
| `temperature_noise.jitter_stddev` | `0.005` | Temperature jitter standard deviation in degrees Celsius |
| Both channels' `fixed_bias` | `0.0` | Constant offset in the channel's units |
| Both channels' `bias_drift_per_s` | `0.0` | Signed offset change per simulated second |
| Both channels' `quantization_step` | `0.0` | Rounding step in channel units; zero disables rounding |

To increase pressure jitter, change `pressure_noise.jitter_stddev` to `5.0` in
the constructor, then rebuild. To restore stationary inputs, set both ideal
rates to zero. To simulate 20 seconds, set `sample_intervals` to `200` with the
default time step; the output will contain 201 readings.

Time step must be finite and positive, and the interval count nonnegative.
Channel settings must be finite; jitter and quantization must be nonnegative.
Bias and drift may have either sign. The application reports invalid timeline
settings at startup; the noise library rejects invalid channel settings.

The ideal value is `initial_value + change_per_second * time_s`. The model then
adds fixed bias, linear drift, and jitter before optional rounding. Each channel
keeps its own model instance throughout the loop. The time argument is absolute
time since simulation start; noisy output is never reused as ideal input.

The default pressure reaches 89000 Pa and temperature reaches 19.5 degrees
Celsius at 10 seconds. These rates are scripted test inputs, not a matched
atmosphere or flight model. The jitter defaults use Bosch's typical
standard-resolution, filter-off noise values as a simplified Gaussian baseline;
they do not calibrate the complete simulated barometer. Standard deviation is
not an error bound: a 2 Pa setting can produce jitter outside plus/minus 2 Pa.
[Bosch BMP390 datasheet, tables 8-9](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp390-ds002.pdf)

## Output and checks

The loop prints a time label followed by ideal and noisy values with units.
Four decimal places control display precision, not hardware accuracy. Simulation
time advances without sleeping between readings. Keeping settings, seeds,
sampling calls, and the toolchain unchanged reproduces the jitter sequence;
exact random values are not guaranteed across toolchains.

The noise library now uses `std::mt19937` in place of `std::minstd_rand`.
The same seeds reproduce the new sequence, not the previous engine's sequence.
This changes individual noisy readings without changing the ideal input curves
or configured error magnitudes. See the [engine documentation](../../sitl/models/noise/README.md)
for details.

For an easy manual check, set both jitter values to zero while leaving the
other errors at zero. Ideal and measured values should match at every time:

```text
Time: 10.0000 s
  Pressure (Pa):   true = 89000.0000   noisy = 89000.0000
  Temperature (C): true =  19.5000   noisy =  19.5000
```

To isolate bias and drift, keep jitter and quantization zero. Set pressure
bias/drift to 5 Pa and 0.1 Pa/s, and temperature bias/drift to 0.2 degrees
Celsius and 0.01 degrees Celsius/s. At 10 seconds, the noisy readings should be
89006 Pa and 19.8 degrees Celsius. Compare against current truth, not the initial
values. Restore the defaults after experimenting if baseline output is wanted.

Run the existing model unit tests using the same build directory:

```sh
cmake --build build/noise-demo-linux --target sitl_noise_test
ctest --test-dir build/noise-demo-linux -R '^sitl_noise_test$' --output-on-failure
```

For WSL PowerShell, prefix commands with `wsl -d Ubuntu --` and use
`build/noise-demo-wsl`. For SITL, use `build/noise-demo-sitl`.

Previously performed independent checks also covered changing inputs, known
bias/drift, repeatability, and baseline/stress jitter statistics. Those used
temporary checking scripts, not a committed verification tool. The demo is not
yet connected to the rocket dynamics, atmosphere harness, fake I2C, or EKF.

## Further references

- [C++ normal distribution specification](https://eel.is/c++draft/rand.dist.norm.normal):
  Gaussian mean and standard deviation used by the noise library
- [C++ stream formatting specification](https://eel.is/c++draft/iostream.format):
  output width and decimal precision
- [Noise-model library documentation](../../sitl/models/noise/README.md):
  algorithm, units, tests, and intended integration boundaries
