add_library(servo_motor INTERFACE)

target_sources(servo_motor INTERFACE
    servo_motor.cc
)

target_include_directories(servo_motor INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(servo_motor INTERFACE
    driver
)