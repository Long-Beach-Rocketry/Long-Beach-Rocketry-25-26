add_library(cli_bsp STATIC
    h723_board.cc
)

target_include_directories(cli_bsp PUBLIC 
  .
  ../
)

target_link_libraries(cli_bsp PUBLIC
    driver
)