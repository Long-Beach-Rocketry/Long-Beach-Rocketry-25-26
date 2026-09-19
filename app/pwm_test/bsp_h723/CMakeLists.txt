add_library(pwm_bsp STATIC
    h723_board.cc
)

target_include_directories(pwm_bsp PUBLIC 
  .
  ../
)

target_link_libraries(pwm_bsp PUBLIC
    driver
    core
)

