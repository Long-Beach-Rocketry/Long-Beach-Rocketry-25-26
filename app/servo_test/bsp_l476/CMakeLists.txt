add_library(servo_bsp STATIC
    l476_board.cc
)

target_include_directories(servo_bsp PUBLIC 
  .
  ../
)

target_link_libraries(servo_bsp PUBLIC
    driver
    core
)

