add_library(bmp390_bsp STATIC
    l476_board.cc
)

target_include_directories(bmp390_bsp PUBLIC 
  .
  ../
)

target_link_libraries(bmp390_bsp PUBLIC
    driver
    core
)

