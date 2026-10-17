add_library(airbrake STATIC
    airbrake.cc
)

target_include_directories(airbrake PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

add_subdirectory(utils)

target_link_libraries(airbrake PUBLIC
    core
    airbrake_utils
)
