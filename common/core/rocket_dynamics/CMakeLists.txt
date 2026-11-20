add_library(rocket_dynamics STATIC
    rocket_dynamics.cc
)

target_include_directories(rocket_dynamics PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)