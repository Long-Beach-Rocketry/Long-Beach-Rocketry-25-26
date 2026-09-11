add_library(inverse_drivers_hw STATIC
    inv_bmp390.cc
    inv_bno055.cc
)

target_include_directories(inverse_drivers_hw
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../interface
)

target_link_libraries(inverse_drivers_hw
    PUBLIC
        inverse
)