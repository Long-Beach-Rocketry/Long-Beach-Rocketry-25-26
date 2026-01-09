
add_library(pps
    pps.cc
    bsp/motor_if.cc
)

target_include_directories(pps PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/bsp)

target_link_libraries(pps PUBLIC bno055)

add_executable(pps_test main.cc)
target_link_libraries(pps_test pps)
set_target_properties(pps pps_test PROPERTIES LINKER_LANGUAGE CXX)
