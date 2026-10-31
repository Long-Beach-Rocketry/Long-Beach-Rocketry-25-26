add_library(mpc STATIC
    mpc.cc
)

target_include_directories(mpc PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(mpc PUBLIC
    rocket_dynamics
    atmosphere
    flight_constants
)