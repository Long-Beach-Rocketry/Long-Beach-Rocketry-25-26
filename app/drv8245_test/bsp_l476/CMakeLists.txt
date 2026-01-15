add_library(drv8245_bsp STATIC
  l476_board.cc
  ${CMAKE_SOURCE_DIR}/common/drivers/platform/drv8245/drv8245.cc
)

target_include_directories(drv8245_bsp PUBLIC 
  .
  ../
  ${CMAKE_SOURCE_DIR}/common/drivers/platform/drv8245
  
)

target_link_libraries(drv8245_bsp PUBLIC
    driver
    core
)

