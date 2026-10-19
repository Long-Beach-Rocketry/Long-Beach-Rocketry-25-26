add_library(MS561101BA03 INTERFACE)

target_sources(MS561101BA03 INTERFACE
    ms561101.cc
)

target_include_directories(MS561101BA03 INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MS561101BA03 INTERFACE
    driver
    utils
)