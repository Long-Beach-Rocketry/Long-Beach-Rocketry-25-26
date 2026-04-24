add_library(can_bsp STATIC
    h723_board.cc
)

target_include_directories(can_bsp PUBLIC 
  .
  ../
)

target_link_libraries(can_bsp PUBLIC
    driver
)

