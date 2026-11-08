# SITL Noise Model

This folder provides a small scalar sensor-noise model for software-in-the-loop
(SITL) simulations. It converts a noise-free value from a physics or atmosphere
model into a sensor-like reading. It does not change the simulated rocket state.

## How it fits into the project

The intended path is:

```text
rocket / atmosphere truth -> ideal sensor value -> NoiseModel -> sensor processing
                                                        -> EKF / flight logic
```

The root CMake build includes `sitl_noise` for both the `native` and `sitl`
presets. `sitl_core` links the library in SITL builds, so a scheduler or sensor
model can use it. The current checkout has not yet connected it to
`rocket_dynamics_sim`, an atmosphere wrapper, fake I2C, or an EKF.

For the rocket-dynamics branch described in the supplied notes, the intended
barometer connection is to apply noise to the ideal pressure and temperature
produced from altitude, before those readings enter simulated sensor
processing. Preserve the physics model's true altitude, velocity, and other
state separately so tests can compare truth with the readings seen by flight
software. The supplied notes specify pressure in Pa and temperature in K from
the atmosphere model, while the BMP390 driver's compensated temperature is in
degrees Celsius; convert temperature at the driver-facing boundary.

## Model behavior

`NoiseModelConfig` values use the same units as the modeled channel:

| Setting | Meaning |
| --- | --- |
| `jitter_stddev` | Standard deviation of fresh Gaussian jitter on each reading. |
| `fixed_bias` | Constant offset applied throughout the run. |
| `bias_drift_per_s` | Signed bias change per second of simulation time. |
| `quantization_step` | Resolution to round to; zero disables quantization. |

For a true value `x` at simulation time `t`, the model forms
`x + fixed_bias + bias_drift_per_s * t + jitter`, then rounds to the nearest
quantization step when one is configured. Pass absolute seconds since the
simulation began to `sample()`; drift is based on that time, not on how many
times the function has been called.

Use one `NoiseModel` instance per channel. The default seed is `1`, which
makes a run repeatable on the same toolchain. Pass a different explicit seed to
each channel when independent jitter sequences are wanted. The model does not
choose calibrated noise settings; use sensor specifications or measured data to
set realistic values.

## Loop demonstration

The [`noise_model_sim` application](../../../app/noise_model_sim_test/main.cc)
prints ideal and noisy pressure/temperature values through 10 simulated seconds.
The comment at the top of its `main.cc` explains Linux/dev-container and
PowerShell-to-WSL commands. Edit [simulation_config.h](../../../app/noise_model_sim_test/simulation_config.h),
then rebuild and restart the application to use new settings.
Output uses labeled blocks with units for each reading. The executable remains
named `noise_model_sim`; its source folder is `app/noise_model_sim_test`.

The configuration groups the timeline, ideal environment, noise parameters, and
channel seeds. [simulation_scenario.h](../../../app/noise_model_sim_test/simulation_scenario.h)
calculates ideal values as `initial_value + change_per_second * time_s`.
The demo constructs its noise models once and supplies current truth and
absolute simulation time on each loop iteration. The library does not depend
on this demonstration's configuration or scenario helper.

| Default setting | Value |
| --- | --- |
| Time step / interval count | 0.1 s / 100 intervals, giving 101 readings |
| Initial pressure / change per second | 90000 Pa / -100 Pa/s |
| Initial temperature / change per second | 20 degrees Celsius / -0.05 degrees Celsius/s |
| Pressure / temperature jitter standard deviation | 2 Pa / 0.005 degrees Celsius |
| Bias, drift, and quantization | Zero in both channels |
| Pressure / temperature seed | 123 / 456 |

The ideal values reach 89000 Pa and 19.5 degrees Celsius at 10 seconds. The
linear rates are scripted test inputs, not a validated atmosphere or flight
model. Pressure and temperature jitter use the typical standard-resolution,
filter-off values in Bosch's tables 8-9 as a simplified Gaussian baseline; these
do not calibrate the complete sensor model. This replaces the earlier demo's
nonzero offsets, drift, and quantization so jitter can be observed separately.
Four decimal places make the small temperature changes visible; they describe
printed precision, not hardware accuracy.

For a zero-error check, set each channel's `jitter_stddev` to zero as well.
Every measured value should match the current ideal value. To isolate bias and
drift, keep jitter/quantization zero and set pressure bias/drift to 5 Pa and
0.1 Pa/s, and temperature bias/drift to 0.2 degrees Celsius and 0.01 degrees
Celsius/s. The readings should then be 90005 Pa / 20.2 degrees Celsius at time
zero, and 89006 Pa / 19.8 degrees Celsius at 10 s. Reset the rates to zero for
a stationary scenario, or change only a noise field to keep truth unchanged.

The same settings, seeds, sampling calls, and standard-library environment
repeat a jitter sequence; exact values are not guaranteed across toolchains.
This demo is not yet connected to the atmosphere harness, fake I2C, or flight
controller and does not run the complete flight simulation.

## Tests

The GoogleTest target checks:

- zero-noise readings preserve the true value;
- fixed bias and time-based drift produce the expected values;
- quantization rounds after bias is applied, including a negative reading;
- the same seed reproduces the same jitter sequence; and
- invalid configuration and input values are rejected.

These tests exercise the scalar model. They do not validate sensor calibration,
atmosphere accuracy, or a complete flight-control simulation.

### Build and run only this test

Run commands from the repository root. Initialize submodules once if needed:

```sh
git submodule update --init --recursive
cmake --preset native
cmake --build build/native --target sitl_noise_test
ctest --test-dir build/native -R sitl_noise_test --output-on-failure
```

To check that the library also builds through the SITL configuration, use:

```sh
cmake --preset sitl
cmake --build build/sitl --target sitl_noise_test
ctest --test-dir build/sitl -R sitl_noise_test --output-on-failure
```

To build all native targets and run all registered native tests, replace the
target-specific build command with `cmake --build build/native` and omit
`-R sitl_noise_test` from CTest. The same pattern works with `build/sitl`.

## Sources

- The [C++ working draft's normal distribution specification](https://eel.is/c++draft/rand.dist.norm.normal)
  describes the mean and standard deviation used for Gaussian jitter.
- The [Bosch BMP390 datasheet](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp390-ds002.pdf),
  tables 6-9, distinguishes sensor resolution from noise and describes how
  oversampling/filter settings affect them. Its temperature footnote identifies
  board/self-heating effects. The demo defaults illustrate errors; they are not
  calibrated to these hardware settings or thermal behavior.
- The [C++ formatting specification](https://eel.is/c++draft/iostream.format)
  describes the output manipulators; printed precision does not change samples.
- The [CMake command-line manual](https://cmake.org/cmake/help/latest/manual/cmake.1.html)
  documents presets, build directories, and building a selected target.
