# Atmosphere: generic ISA physics (atmosphere.cc) + the Atmosphere Model
# pipeline box (atmosphere_model.cc, true altitude -> true barometer values).
# Pure math, no I/O, no hardware dependency, so this library is built for
# every target, same reasoning as common/core/rocket_dynamics.

add_library(atmosphere STATIC
    atmosphere.cc
    atmosphere_model.cc
)

target_include_directories(atmosphere PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)