add_library(airbrake_bsp STATIC
    h723_board.cc
)

target_include_directories(airbrake_bsp PUBLIC 
  .
  ../
)

target_link_libraries(airbrake_bsp PUBLIC
    driver
    container
    airbrake
    core
)