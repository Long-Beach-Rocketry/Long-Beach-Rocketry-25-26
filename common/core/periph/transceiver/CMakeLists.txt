add_library(rs485 INTERFACE)

target_include_directories(rs485 INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(rs485 INTERFACE
    driver
)