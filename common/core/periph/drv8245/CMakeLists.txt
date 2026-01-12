add_library(drv8245 INTERFACE)

target_sources(drv8245 INTERFACE
    drv8245.cc
)

target_include_directories(drv8245 INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(drv8245 INTERFACE
    driver
)