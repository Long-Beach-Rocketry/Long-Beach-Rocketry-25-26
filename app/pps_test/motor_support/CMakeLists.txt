add_library(motor_support STATIC
    hw/dc_motor.cc
)

target_include_directories(motor_support
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)


