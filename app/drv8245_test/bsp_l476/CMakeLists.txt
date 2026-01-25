add_library(drv8245_bsp STATIC
  l476_board.cc
)

target_include_directories(drv8245_bsp PUBLIC 
  .
  ../
  ${CMAKE_SOURCE_DIR}/common/core/periph/drv8245
)

target_link_libraries(drv8245_bsp PUBLIC
    driver
    core
)

