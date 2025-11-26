add_library(bmp390 INTERFACE)

target_sources(bmp390 INTERFACE
    bmp390.cc
)

target_include_directories(bmp390 INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(bmp390 INTERFACE
    driver
)