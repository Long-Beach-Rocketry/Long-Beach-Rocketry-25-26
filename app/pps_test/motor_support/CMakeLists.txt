add_library(motor_support STATIC
    hw/motor_control.cc
)

target_include_directories(motor_support
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)


