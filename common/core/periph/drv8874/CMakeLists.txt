add_library(drv8874 INTERFACE)

target_sources(drv8874 INTERFACE
    drv8874.cc
)

target_include_directories(drv8874 INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(drv8874 INTERFACE 
    driver
)