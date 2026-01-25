add_library(pps
    pps.cc
    motor_support/dc_motor.cc
)

target_include_directories(pps PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    #${CMAKE_CURRENT_SOURCE_DIR}/bsp_pps/hw not until I have BSP
    ${CMAKE_CURRENT_SOURCE_DIR}/helpers
    ${CMAKE_CURRENT_SOURCE_DIR}/motor_support
    ${CMAKE_SOURCE_DIR}/common/core/periph/drv8245
)
target_link_libraries(pps PUBLIC bno055)

add_executable(pps_test main.cc)
target_link_libraries(pps_test pps)
set_target_properties(pps pps_test PROPERTIES LINKER_LANGUAGE CXX)

