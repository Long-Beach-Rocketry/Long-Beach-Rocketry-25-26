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
