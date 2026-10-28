add_library(airbrake_utils STATIC
    airbrake_utils.cc
)

target_include_directories(airbrake_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(airbrake_utils PUBLIC
    core
)