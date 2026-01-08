add_library(pps
    pps.cc
    motor_support/dc_motor.cc
)

target_include_directories(pps PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/bsp_pps/hw)

target_link_libraries(pps PUBLIC bno055)

add_executable(pps_app main.cc)
target_link_libraries(pps_app pps)
set_target_properties(pps pps_app PROPERTIES LINKER_LANGUAGE CXX)
