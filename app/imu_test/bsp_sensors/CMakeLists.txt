add_library(bsp_sensors imu_app_bsp.cc)
add_library(bsp_sensors imu_app_bsp.h)

target_include_directories(bsp_sensors PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(bsp_sensors PUBLIC core)