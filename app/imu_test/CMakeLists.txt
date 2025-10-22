add_executable(imu_test main.cc)
target_link_libraries(imu_test PRIVATE bsp_sensors core)