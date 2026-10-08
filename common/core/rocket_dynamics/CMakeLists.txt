# Rocket Dynamics Model: 2D point-mass flight physics. Pure math, no I/O and
# no hardware dependency, so this library is built for every target -- it can
# be linked by hardware targets too (e.g. an onboard apogee predictor using
# the same physics), not just by NATIVE/SITL simulation.

add_library(rocket_dynamics STATIC
    rocket_dynamics.cc
)

target_include_directories(rocket_dynamics PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)