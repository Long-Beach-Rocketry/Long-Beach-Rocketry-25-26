add_library(w25q INTERFACE)

target_sources(w25q INTERFACE
    w25q.cc
)

target_include_directories(w25q INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(w25q INTERFACE
    driver
)