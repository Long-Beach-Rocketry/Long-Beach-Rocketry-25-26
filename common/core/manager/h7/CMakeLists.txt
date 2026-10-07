add_library(st_sensor_manager STATIC
    st_sensor_manager.cc
)

target_include_directories(st_sensor_manager PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(st_sensor_manager PUBLIC
    sensor_manager
    core
    driver
)
