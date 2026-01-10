add_library(bsp_l476 STATIC
    l476_board.cc
)

target_include_directories(bsp_l476 PUBLIC 
  .
  ../
)

target_link_libraries(bsp_l476 PUBLIC
    driver
)

