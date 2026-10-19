add_library(kalman_bsp STATIC
    h723_board.cc
)

target_include_directories(kalman_bsp PUBLIC 
  .
  ../
  ${CMAKE_SOURCE_DIR}/common/core/periph/bno055
  ${CMAKE_SOURCE_DIR}/common/core/periph/bmp390
  ${CMAKE_SOURCE_DIR}/common/drivers/utils
  ${CMAKE_SOURCE_DIR}/common/core/math
)

target_link_libraries(kalman_bsp PUBLIC
  driver
  core
)

