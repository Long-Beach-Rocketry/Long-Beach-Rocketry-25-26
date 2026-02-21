<<<<<<< HEAD
add_library(bno055_bsp STATIC
    l476_board.cc
)

target_include_directories(bno055_bsp PUBLIC 
  .
  ../
  ${CMAKE_SOURCE_DIR}/common/core/periph/bno055
  ${CMAKE_SOURCE_DIR}/common/drivers/utils
  ${CMAKE_SOURCE_DIR}/common/core/math
)

target_link_libraries(bno055_bsp PUBLIC
  driver
  core
)

=======
add_library(bsp_sensors imu_app_bsp.cc)

target_include_directories(bsp_sensors
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../bsp_l476
    PRIVATE ${CMAKE_SOURCE_DIR}/common/drivers/platform/bno055
)

target_link_libraries(bsp_sensors PUBLIC core bno055)
>>>>>>> 319d772 (ekf)
