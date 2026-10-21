add_library(ms561101ba03 INTERFACE)

target_sources(ms561101ba03 INTERFACE
    ms561101.cc
)

target_include_directories(ms561101ba03 INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ms561101ba03 INTERFACE
    driver
    utils
)