add_library(drv8874_bsp STATIC
  l476_board.cc
)

target_include_directories(drv8874_bsp PUBLIC 
  .
  ../
  ${CMAKE_SOURCE_DIR}/common/core/periph/drv8874
)

target_link_libraries(drv8874_bsp PUBLIC
    driver
    core
)

