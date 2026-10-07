# Rocket Dyanmics Model + Atmosphere Model for SITL

Simulates one rocket flight, with and without airbrakes, and prints the results to the terminal. Meant to follow the SITL pipeline: "Rocket Dynamic Model" and "Atmosphere Model". Everything else in the pipeline (airbrake logic, MPC, sensor noise) is stand-in inside "main.cc" or not created yet and still needs to be implemented.

## Running main.cc
From the repo root:
git submodule update --init --recursive   # one-time setup
cmake --preset native
cd build/native
ninja rocket_dynamics_sim
./app/rocket_dynamics_sim/rocket_dynamics_sim

## Folders
common/core/rocket_dynamics/    Rocket Dynamics Model (library)
common/core/atmosphere/         Atmosphere physics + Atmosphere Model (library)
app/rocket_dynamics_sim/        main.cc, the program that runs everything

- "atmosphere" files are general purpose air physics: temperature, pressure and density at a height. "atmosphere_model" files wrap around atmsophere files and returns (noise-free) pressure and temperature at an altitude.
- "rocket_dynamics" files are the rocket's physics. They define the rocket's state (position, velocity, acceleration, mass), its fixed properties (drag coefficients, surface area, number of flaps) and inputs each step (thrust, gravity, air density, and flap angle). 

## Sources
- Rocket Dynamic Model follows the research paper linked as https://arc.aiaa.org/doi/epdf/10.2514/6.2024-83924.
- Atmosphere Model follows standard International Standard Atmosphere (ISA) troposphere model, basically lotta google searches and etc.

## What's mock data or not included here
- Rocket mass, motor thrust curve, drag coefficients and flap areas are placeholders.
- Ground conditions are standard-atmosphere values to a 1200m elevation, not measured at launch spot, will be taken during launch day.
- The rocket always points straight up, so horizontal position stays 0 for the whole flight.
- No noise model so data is perfect, in real life it won't be like that.
- The flap command is a simple on/off rule. The MPS will replace it.

## Notes for the sensor-noise work
- Atmosphere Model output is in Pascals and Kelvin, but the BMP390 driver reports in Calsius, so convert before building raw sensor values.
- Noise model should wrap around "atmosphere_model"