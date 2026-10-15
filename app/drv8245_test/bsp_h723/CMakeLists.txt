add_library(drv8245_bsp STATIC
    h723_board.cc
)

target_include_directories(drv8245_bsp PUBLIC 
  .
  ../
)

target_link_libraries(drv8245_bsp PUBLIC
    driver
    core
    drv8245
)

