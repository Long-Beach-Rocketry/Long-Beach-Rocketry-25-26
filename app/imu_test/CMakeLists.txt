add_subdirectory(bsp_sensors)

add_executable(imu_test main.cc)

# Ensure headers from bsp_sensors are visible when building the executable
target_include_directories(imu_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/bsp_sensors)

target_link_libraries(imu_test PRIVATE bsp_sensors core)