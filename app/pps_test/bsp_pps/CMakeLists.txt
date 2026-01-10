add_library(bsp_pps_hw
    hw/pps_hw.cc
)

target_include_directories(bsp_pps_hw
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/hw
)


