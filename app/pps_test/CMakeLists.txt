add_library(pps
    pps.cc
    motor_support/motor_if.cc
)

target_include_directories(pps PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/bsp_pps/hw)

target_link_libraries(pps PUBLIC bno055)

add_executable(pps_test main.cc)
target_link_libraries(pps_test pps)
set_target_properties(pps pps_test PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(pps_test PRIVATE ${CMAKE_SOURCE_DIR}/common/drivers/platform/bno055)
target_include_directories(pps_test PRIVATE ${CMAKE_SOURCE_DIR}/common/drivers/utils)
