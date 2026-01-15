add_library(bno055_bsp STATIC
    l476_board.cc
)

target_include_directories(bno055_bsp PUBLIC 
  .
  ../
  ${CMAKE_SOURCE_DIR}/common/drivers/platform/bno055
  ${CMAKE_SOURCE_DIR}/common/drivers/utils
  ${CMAKE_SOURCE_DIR}/common/core/math
)

target_link_libraries(bno055_bsp PUBLIC
  driver
  core
  bno055
)

