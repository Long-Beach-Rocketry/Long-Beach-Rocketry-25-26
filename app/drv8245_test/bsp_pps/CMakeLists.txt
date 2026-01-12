add_library(bsp_pps STATIC
    pps_board.cc
)

target_include_directories(bsp_pps PUBLIC 
  .
  ../
)

target_link_libraries(bsp_pps PUBLIC
    driver
)

