add_library(bsp_sensors imu_app_bsp.cc)

target_include_directories(bsp_sensors
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../bsp_l476
    PRIVATE ${CMAKE_SOURCE_DIR}/common/drivers/platform/bno055
)

target_link_libraries(bsp_sensors PUBLIC core bno055)
