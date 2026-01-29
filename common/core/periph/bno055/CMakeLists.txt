add_library(bno055 INTERFACE)

target_sources(bno055 INTERFACE
    bno055_imu.cc
)

target_include_directories(bno055 INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common/core/math
    ${CMAKE_SOURCE_DIR}/common/drivers/utils
)

target_link_libraries(bno055 INTERFACE
    driver
)