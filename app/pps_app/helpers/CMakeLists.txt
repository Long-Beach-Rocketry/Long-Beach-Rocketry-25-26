add_library(helpers STATIC
    hw/pps_helpers.cc
)

target_include_directories(helpers
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)


