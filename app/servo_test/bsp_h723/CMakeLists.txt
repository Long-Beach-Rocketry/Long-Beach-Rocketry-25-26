add_library(servo_bsp STATIC
    h723_board.cc
)

target_include_directories(servo_bsp PUBLIC
    .
    ../
)

target_link_libraries(servo_bsp PUBLIC
    hal
    servo_motor
    utils
)
