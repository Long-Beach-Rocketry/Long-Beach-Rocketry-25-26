add_library(sx1262 INTERFACE)

target_sources(sx1262 INTERFACE
    sx1262.cc
)

target_include_directories(sx1262 INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(sx1262 INTERFACE
    driver
)